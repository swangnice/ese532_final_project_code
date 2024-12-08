#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS


#include "EventTimer.h"
#include <CL/cl2.hpp>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <vector>

#include "../Server/server.h"
#include "../Server/chunk.h"
#include "../Server/cdc.h"
#include "../Server/sha.h"
#include "../Server/lzw.h"
//#include "host.h"
#include "utils.h"

// max number of elements we can get from ethernet
#define NUM_ELEMENTS 16384
#define HEADER 2

#define NUM_PACKETS 8
#define pipe_depth 4
#define DONE_BIT_L (1 << 7)
#define DONE_BIT_H (1 << 15)
#define CHUNKS_IN_SINGLE_KERNEL 60
#define MAX_CHUNK_SIZE 4096
#define MAX_FILE_BUFFER_SIZE 4096

using namespace std;
int offset = 0;
unsigned char* file;

void handle_input(int argc, char* argv[], int* blocksize) {
	int x;
	extern char *optarg;

	while ((x = getopt(argc, argv, ":b:")) != -1) {
		switch (x) {
		case 'b':
			*blocksize = atoi(optarg);
			printf("blocksize is set to %d optarg\n", *blocksize);
			break;
		case ':':
			printf("-%c without parameter\n", optopt);
			break;
		}
	}
}

void checkErr(cl_int err, const char* name) {
    if (err != CL_SUCCESS) {
        std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char** argv)
{
    EventTimer timer;
// ------------------------------------------------------------------------------------
// Step 1: Initialize the OpenCL environment 
// ------------------------------------------------------------------------------------ 
    timer.add("OpenCL Initialization");
    cl_int err;
    std::string binaryFile = argv[1];
    unsigned fileBufSize;    
    std::vector<cl::Device> devices = get_xilinx_devices();
    devices.resize(1);
    cl::Device device = devices[0];
    cl::Context context(device, NULL, NULL, NULL, &err);
    char* fileBuf = read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    cl::Program program(context, devices, bins, NULL, &err);
    cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
    
    cl::Kernel lzw_kernel = cl::Kernel(program,"lzw_compress_hw", &err);
    
// ------------------------------------------------------------------------------------
// Step 2: Create buffers and initialize test values
// ------------------------------------------------------------------------------------

    unsigned char* lzw_s1;
    int *lzw_length;
    uint8_t *lzw_is_dup;
    int *lzw_dup_index;
	uint8_t *lzw_temp_out_buffer;
	unsigned int *lzw_temp_out_buffer_size;

    cl::Buffer lzw_s1_buf = cl::Buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY,  sizeof(unsigned char) * 2048, NULL, &err);
    cl::Buffer lzw_length_buf = cl::Buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY, sizeof(int), NULL, &err);
	cl::Buffer lzw_is_dup_buf = cl::Buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY, sizeof(uint8_t), NULL, &err);
	cl::Buffer lzw_dup_index_buf = cl::Buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY, sizeof(int), NULL, &err);
	cl::Buffer lzw_temp_out_buffer_buf = cl::Buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(uint8_t) * 2048, NULL, &err);
	cl::Buffer lzw_temp_out_buffer_size_buf = cl::Buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(unsigned int), NULL, &err);
	
    lzw_s1 = (unsigned char *)q.enqueueMapBuffer(lzw_s1_buf, CL_TRUE, CL_MAP_WRITE, 0, sizeof(unsigned char) * 2048);
    lzw_length = (int *)q.enqueueMapBuffer(lzw_length_buf, CL_TRUE, CL_MAP_WRITE, 0, sizeof(int));
	lzw_is_dup = (uint8_t *)q.enqueueMapBuffer(lzw_is_dup_buf, CL_TRUE, CL_MAP_WRITE, 0, sizeof(uint8_t));
	lzw_dup_index = (int *)q.enqueueMapBuffer(lzw_dup_index_buf, CL_TRUE, CL_MAP_WRITE, 0, sizeof(int));
	lzw_temp_out_buffer = (uint8_t *)q.enqueueMapBuffer(lzw_temp_out_buffer_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(uint8_t) * 2048);
	lzw_temp_out_buffer_size = (unsigned int *)q.enqueueMapBuffer(lzw_temp_out_buffer_size_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(unsigned int));
	
// ------------------------------------------------------------------------------------
// Step 3: Run the kernel
// ------------------------------------------------------------------------------------
    stopwatch ethernet_timer;
	stopwatch cdc_timer;
	stopwatch sha_timer;
	stopwatch dedup_timer;
	stopwatch lzw_timer;
	stopwatch overall_timer;
	
	unsigned char* input[NUM_PACKETS];
	int writer = 0;
	int done = 0;
	int length = 0;
	int count = 0;
	ESE532_Server server;
    int blocksize = BLOCKSIZE;

	// set blocksize if decalred through command line
	handle_input(argc, argv, &blocksize);

	file = (unsigned char*) malloc(sizeof(unsigned char) * 70000000);
	if (file == NULL) {
		printf("help\n");
	}

	for (int i = 0; i < NUM_PACKETS; i++) {
		input[i] = (unsigned char*) malloc(
				sizeof(unsigned char) * (NUM_ELEMENTS + HEADER));
		if (input[i] == NULL) {
			std::cout << "aborting " << std::endl;
			return 1;
		}
	}

	server.setup_server(blocksize);

	writer = pipe_depth;
	ethernet_timer.start();
	server.get_packet(input[writer]);
	ethernet_timer.stop();
	count++;

	// get packet
	unsigned char* buffer = input[writer];

	done = buffer[1] & DONE_BIT_L;
	length = buffer[0] | (buffer[1] << 8);
	length &= ~DONE_BIT_H;

	overall_timer.start();
	memcpy(&file[offset], &buffer[HEADER], length);

    unsigned int estimated_chunks = 15360 / WIN_SIZE + 1;

	unsigned char **chunks = NULL;
    unsigned int chunk_count = 0;
    int *chunk_sizes = NULL;
    chunks = (unsigned char **)malloc(sizeof(unsigned char *) * estimated_chunks);
	chunk_count = 0;
    chunk_sizes = (int *)malloc(sizeof(int) * estimated_chunks);
	cdc_timer.start();
	gear_based_fastcdc(&buffer[HEADER], length, &chunks, &chunk_count, &chunk_sizes);
	//rabin_fingerprint_cdc(&buffer[HEADER], length, &chunks, &chunk_count, &chunk_sizes);
	cdc_timer.stop();
	offset += length;
	writer++;
//Get all UDP Packets and CDC
	while (!done) {
		// reset ring buffer
		if (writer == NUM_PACKETS) {
			writer = 0;
		}

		//ethernet_timer.start();
		server.get_packet(input[writer]);
		//ethernet_timer.stop();

		count++;

		// get packet
		unsigned char* buffer = input[writer];

		// decode
		done = buffer[1] & DONE_BIT_L;
		length = buffer[0] | (buffer[1] << 8);
		length &= ~DONE_BIT_H;
		//printf("length: %d offset %d\n",length,offset);
		memcpy(&file[offset], &buffer[HEADER], length);
		cdc_timer.start();
		//rabin_fingerprint_cdc(&buffer[HEADER], length, &chunks, &chunk_count, &chunk_sizes);
		gear_based_fastcdc(&buffer[HEADER], length, &chunks, &chunk_count, &chunk_sizes);
		cdc_timer.stop();
		
		offset += length;
		writer++;
	}

    FILE *outfd = fopen("output_cpu.bin", "wb");
	int bytes_written = fwrite(&file[0], 1, offset, outfd);
	printf("write file with %d\n", bytes_written);
	fclose(outfd);

	//print all chunks
	// printf("Chunk count: %u\n", chunk_count);
	// printf("Chunk sizes:\n");
	// for (unsigned int i = 0; i < chunk_count; i++) {
	// 	printf("Chunk %u size: %u bytes\n", i, chunk_sizes[i]);
	// }
	// printf("Chunk data (as string):\n");
	// for (unsigned int i = 0; i < chunk_count; i++) {
	// 	printf("Chunk %u: ", i);
	// 	printf("%.*s\n", chunk_sizes[i], (char *)chunks[i]);
	// }

    //SHA256 hash
	sha_timer.start();
    uint8_t sha256_output[chunk_count][32];

	for (unsigned int i = 0; i < chunk_count; i++) {
		unsigned char *temp_chunk_data = chunks[i];
    	unsigned int temp_chunk_size = chunk_sizes[i];

		unsigned char compressed_data[SHA_BLOCKSIZE];
    	int compressed_size = rle_compress((const unsigned char*)temp_chunk_data, temp_chunk_size, compressed_data, SHA_BLOCKSIZE);

		uint8_t temp_sha256_output[32]; 
		calculate_sha256(compressed_data, compressed_size, temp_sha256_output);

		std::cout << "SHA-256 Hash of Compressed Data: ";
		for (int i = 0; i < 32; i++) {
			printf("%02x", temp_sha256_output[i]);
		}
		std::cout << std::endl;
		memcpy(sha256_output[i], temp_sha256_output, 32);
		
	}
	sha_timer.stop();

    // Deduplication
	dedup_timer.start();
	int *dup_flag = (int *)malloc(sizeof(int) * chunk_count);
	int *dup_index = (int *)malloc(sizeof(int) * chunk_count);
	dup_flag[0] = 0;	
	dup_index[0] = 0;
	int undup_count = 1;
	for (unsigned int i = 1; i < chunk_count; i++) {				
		dup_flag[i] = 0;	//un duplicated
		dup_index[i] = i;
		for (unsigned int j = 0; j < i; j++) {
			if (memcmp(sha256_output[i], sha256_output[j], 32) == 0) {
				dup_flag[i] = 1;
				dup_index[i] = j;
				break;
			}
		}
		undup_count++;
	}
	for (unsigned int i = 0; i < chunk_count; i++) {
		printf("Chunk %u: %d, duplicated with %d\n", i, dup_flag[i], dup_index[i]);
	}
	dedup_timer.stop();

	for (unsigned int i = 0; i < chunk_count; i++) {
		if (dup_flag[i] == 0) {
			printf("Chunk %u:", i);
			printf("chunk size: %d\n", chunk_sizes[i]);
		}
	}

    // LZW on HW
    // uint16_t temp_lzw_compressed_output[chunk_count][2048]; // 假设 MAX_CHUNK_SIZE 是单个块的最大大小
    // int temp_output_index[chunk_count];
    // uint8_t lzw_compressed_output[undup_count][2048];
    // int compressed_data_size[undup_count];
    lzw_timer.start();
    uint8_t *out_buffer;
    out_buffer = (uint8_t *)malloc(sizeof(uint8_t) * 1024 * 512); //    512KB Here, we assume that the maximum size of the whole compressed data is 512KB
    uint8_t *temp_out_buffer;
    temp_out_buffer = (uint8_t *)malloc(sizeof(uint8_t) * 1024 * 2); //   2KB Here, we assume that the maximum size of the compressed data of one chunk is 2KB
    unsigned int temp_out_buffer_size = 0;
    size_t out_offset = 0;

    for (unsigned int i = 0; i < chunk_count; i++) {
        //TODO: Seperate dep and undep chunks
        //if (dup_flag[i] == 0) {
            //lzw_compress(chunks[i], &chunk_sizes[i], temp_lzw_compressed_output[i], &temp_output_index[i]);
            memcpy(lzw_s1, chunks[i], chunk_sizes[i]);
			//printf("chunk size: %d\n", chunk_sizes[i]);
            *lzw_length = chunk_sizes[i];
            printf("chunk size: %d\n", *lzw_length);
            *lzw_is_dup = dup_flag[i];
            printf("dup_flag: %d\n", *lzw_is_dup);
            *lzw_dup_index = dup_index[i];
            printf("dup_index: %d\n", *lzw_dup_index);

            printf("passed in lzw_compress\n");
			//printf("chunk size: %d\n", *lzw_length);
			// printf("Chunk %u:", i);
			// for (int j = 0; j < *lzw_length; j++) { // `total_size` 是 lzw_s1 的总长度
			// 	printf("%c", lzw_s1[j]);
			// }
			// printf("\n");

			//printf("begin to write in buffer\n");

			lzw_kernel.setArg(0, lzw_s1_buf);
            lzw_kernel.setArg(1, lzw_length_buf);
            lzw_kernel.setArg(2, lzw_is_dup_buf);
            lzw_kernel.setArg(3, lzw_dup_index_buf);
            lzw_kernel.setArg(4, lzw_temp_out_buffer_buf);
            lzw_kernel.setArg(5, lzw_temp_out_buffer_size_buf);

            printf("set kernel args end\n");

            std::vector<cl::Event> write_events;
            std::vector<cl::Event> exec_events;

            cl::Event write_ev;
            cl::Event exec_ev;
            cl::Event read_ev;

			//printf("begin queue\n");
            // q.enqueueMigrateMemObjects({lzw_s1_buf, lzw_length_buf, lzw_is_dup_buf, lzw_dup_index_buf}, 0, NULL, &write_ev);
            // printf("test 1");

            cl_int e = CL_SUCCESS;
            e = q.enqueueMigrateMemObjects({lzw_s1_buf, lzw_length_buf, lzw_is_dup_buf, lzw_dup_index_buf}, 0, NULL, &write_ev);
            if (err != CL_SUCCESS) {
                std::cerr << "OpenCL Error: " << e << std::endl;
            }
            q.finish();
            
            // Create a vector for the event dependency
            write_events.push_back(write_ev);
            q.enqueueTask(lzw_kernel, &write_events, &exec_ev);
            printf("test 2");
            
            // Create another vector for the event dependency
            exec_events.push_back(exec_ev);
            q.enqueueMigrateMemObjects({lzw_temp_out_buffer_buf, lzw_temp_out_buffer_size_buf}, CL_MIGRATE_MEM_OBJECT_HOST, &exec_events, &read_ev);
            printf("test 3");
            // Wait for all kernels to finish
            q.finish();
            printf("finished queue\n");

        memcpy(out_buffer + out_offset, lzw_temp_out_buffer, *lzw_temp_out_buffer_size);
        out_offset += *lzw_temp_out_buffer_size;
			//printf("temp_output_index: %d\n", temp_output_index[i]);
        //}
    }
	lzw_timer.stop();

	overall_timer.stop();



//Store in a file
    FILE* out_file = fopen("compressed_data.bin", "wb");
    if (out_file == NULL) {
        perror("Failed to open file");
    }
    size_t written = fwrite(out_buffer, 1, out_offset, out_file);
    if (written != out_offset) {
        perror("Failed to write the entire compressed data to file");
    }
    fclose(out_file);





    for (int i = 0; i < NUM_PACKETS; i++) {
		free(input[i]);
	}

	for (unsigned int i = 0; i < chunk_count; ++i) {
        free(chunks[i]);
    }
    free(chunks);
    free(chunk_sizes);

	delete[] fileBuf;

	std::cout << "\n--------------- Key Throughputs ---------------" << std::endl;
	float ethernet_latency = ethernet_timer.latency() / 1000.0;
	float cdc_latency = cdc_timer.latency() / 1000.0;
	float sha_latency = sha_timer.latency() / 1000.0;
	float dedup_latency = dedup_timer.latency() / 1000.0;
	float lzw_latency = lzw_timer.latency() / 1000.0;
	float overall_latency = overall_timer.latency() / 1000.0;

	float input_throughput = (bytes_written * 8 / 1000000.0) / ethernet_latency; // Mb/s
	float cdc_throughput = (bytes_written * 8 / 1000000.0) / cdc_latency; // Mb/s
	float sha_throughput = (bytes_written * 8 / 1000000.0) / sha_latency; // Mb/s
	float dedup_throughput = (bytes_written * 8 / 1000000.0) / dedup_latency; // Mb/s
	float lzw_throughput = (bytes_written * 8 / 1000000.0) / lzw_latency; // Mb/s
	float overall_throughput = (bytes_written * 8 / 1000000.0) / overall_latency; // Mb/s


	std::cout << "Input Throughput to Encoder: " << input_throughput << " Mb/s."
			<< " (Latency: " << ethernet_latency << "s)." << std::endl;
	std::cout << "CDC Throughput: " << cdc_throughput << " Mb/s." << " (Latency: " << cdc_latency << "s)." << std::endl;
	std::cout << "SHA Throughput: " << sha_throughput << " Mb/s." << " (Latency: " << sha_latency << "s)." << std::endl;
	std::cout << "Deduplication Throughput: " << dedup_throughput << " Mb/s." << " (Latency: " << dedup_latency << "s)." << std::endl;
	std::cout << "LZW Throughput: " << lzw_throughput << " Mb/s." << " (Latency: " << lzw_latency << "s)." << std::endl;
	std::cout << "Overall Throughput: " << overall_throughput << " Mb/s." << " (Latency: " << overall_latency << "s)." << std::endl;


    return 0;

}
