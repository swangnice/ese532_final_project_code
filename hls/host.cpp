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


int convert_output(uint16_t in[], uint8_t out[], int input_size){
    //header
    int output_size = 0;

    int adjusted_input_size = input_size - (input_size % 2);

    for(int i = 0; i < adjusted_input_size; i+=2){
        //printf("in[i]: %hu\n", in[i]);
        //printf("in[i+1]: %hu\n", in[i+1]);
        

        out[output_size] = (in[i]>>4) & 0xff;
        //printf("out[1]: %u\n", static_cast<unsigned int>(out[output_size]));
        output_size++;
        out[output_size] = ((in[i] << 4) & 0xf0) | ((in[i+1] >> 8) & 0x0f);
        //printf("out[2]: %u\n", static_cast<unsigned int>(out[output_size]));
        output_size++;
        out[output_size] = (in[i+1]) & 0xff;
        //printf("out[3]: %u\n", static_cast<unsigned int>(out[output_size]));
        output_size++;
    }

    if (input_size % 2 != 0) {
        out[output_size] = (in[adjusted_input_size] >> 4) & 0xFF;
        output_size++;
        out[output_size] = (in[adjusted_input_size] << 4) & 0xF0;
        output_size++;
    }

    return output_size;
}

int main(int argc, char** argv)
{
	//pin_main_thread_to_cpu0();
// Initialize an event timer we'll use for monitoring the application
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

    //implement multi-kernels
    cl::Kernel lzw_kernel;
    lzw_kernel = cl::Kernel(program,"lzw_compress", &err);

// ------------------------------------------------------------------------------------
// Step 2: Create buffers and initialize test values
// ------------------------------------------------------------------------------------
    timer.add("Allocate contiguous OpenCL buffers");
    // Create the buffers and allocate memory   
    cl::Buffer lzw_chunks_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY,  sizeof(unsigned char) * MAX_CHUNK_SIZE, NULL, &err);
    cl::Buffer lzw_chunks_length_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY, sizeof(int), NULL, &err);
    cl::Buffer lzw_file_buffer_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY,  sizeof(uint16_t) * MAX_FILE_BUFFER_SIZE, NULL, &err);
    cl::Buffer lzw_total_bytes_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY,  sizeof(int), NULL, &err);

    unsigned char* lzw_chunks;
    int* lzw_chunks_length;
    uint16_t* lzw_file_buffer;
    int* lzw_total_bytes;

    lzw_chunks = (unsigned char*)q.enqueueMapBuffer(lzw_chunks_buf, CL_TRUE, CL_MAP_WRITE, 0, sizeof(unsigned char) * MAX_CHUNK_SIZE);
    lzw_chunks_length = (int*)q.enqueueMapBuffer(lzw_chunks_length_buf, CL_TRUE, CL_MAP_WRITE, 0, sizeof(int) );
    lzw_file_buffer = (uint16_t*)q.enqueueMapBuffer(lzw_file_buffer_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(uint16_t) * MAX_FILE_BUFFER_SIZE);
    lzw_total_bytes = (int*)q.enqueueMapBuffer(lzw_total_bytes_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(int));

    timer.add("Populating buffer inputs");

// ------------------------------------------------------------------------------------
// Step 3: Run the kernel
// ------------------------------------------------------------------------------------
	std::cout << argv[1] << std::endl;
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

	// std::vector<cl::Event> read_events[NUM_KERNELS];

	// default is 2k
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
	server.get_packet(input[writer]);
	count++;

    // get packet
	unsigned char* buffer = input[writer];

	// decode
	done = buffer[1] & DONE_BIT_L;
	length = buffer[0] | (buffer[1] << 8);
	length &= ~DONE_BIT_H;
	// printing takes time so be weary of transfer rate
	//printf("length: %d offset %d\n",length,offset);

	// we are just memcpy'ing here, but you should call your
	// top function here.
	overall_timer.start();
	memcpy(&file[offset], &buffer[HEADER], length);
	//printf("%.*s\n", length, &buffer[HEADER]);
	unsigned char **chunks = NULL;
    unsigned int chunk_count = 0;
    int *chunk_sizes = NULL;
	chunk_count = 0;
    unsigned int estimated_chunks = 15360 / WIN_SIZE + 1;
    chunks = (unsigned char **)malloc(sizeof(unsigned char *) * estimated_chunks);
    chunk_sizes = (int *)malloc(sizeof(unsigned int) * estimated_chunks);
	cdc_timer.start();
	cdc(&buffer[HEADER], length, &chunks, &chunk_count, &chunk_sizes);




	offset += length;
	writer++;

    //last message
	while (!done) {
		// reset ring buffer
		if (writer == NUM_PACKETS) {
			writer = 0;
		}

		ethernet_timer.start();
		server.get_packet(input[writer]);
		ethernet_timer.stop();

		count++;

		// get packet
		unsigned char* buffer = input[writer];

		// decode
		done = buffer[1] & DONE_BIT_L;
		length = buffer[0] | (buffer[1] << 8);
		length &= ~DONE_BIT_H;
		//printf("length: %d offset %d\n",length,offset);
		memcpy(&file[offset], &buffer[HEADER], length);
		
		cdc(&buffer[HEADER], length, &chunks, &chunk_count, &chunk_sizes);
		

		offset += length;
		writer++;
	}

        cdc_timer.stop();

	FILE *outfd = fopen("output_cpu.bin", "wb");
	int bytes_written = fwrite(&file[0], 1, offset, outfd);
	printf("write file with %d\n", bytes_written);
	fclose(outfd);

        sha_timer.start();
	uint8_t sha256_output[chunk_count][32];

	for (unsigned int i = 0; i < chunk_count; i++) {
	unsigned char *temp_chunk_data = chunks[i];
    	int temp_chunk_size = chunk_sizes[i];

	unsigned char compressed_data[SHA_BLOCKSIZE];
    	int compressed_size = rle_compress((const unsigned char*)temp_chunk_data, temp_chunk_size, compressed_data, SHA_BLOCKSIZE);

		//std::cout << "Original Size: " << temp_chunk_size << " bytes\n";
		//std::cout << "Compressed Size: " << compressed_size << " bytes\n";

		// 计算压缩数据的 SHA-256 哈希
		uint8_t temp_sha256_output[32]; // SHA-256 输出为 32 字节的 uint8_t 数组
		calculate_sha256(compressed_data, compressed_size, temp_sha256_output);

		// 输出 SHA-256 哈希结果
		std::cout << "SHA-256 Hash of Compressed Data: ";
		for (int i = 0; i < 32; i++) {
			printf("%02x", temp_sha256_output[i]);  // 打印每个字节
		}
		std::cout << std::endl;


		memcpy(sha256_output[i], temp_sha256_output, 32);
		
	}

	sha_timer.stop();
	//int dict[MAX_DICT_SIZE][256];

	//init_dict(dict);
	
	//deduplication and assign lzw header
	dedup_timer.start();
	uint8_t dup_flag[chunk_count];
	int dup_index[chunk_count];
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

    	//gunsigned char* buffer_out = (unsigned char*) malloc(sizeof(unsigned char) * 70000000);
	//compress unduplicated chunks
	lzw_timer.start();
	uint8_t lzw_compressed_output[undup_count][2048];
	
	int compressed_data_size[undup_count];
	
	            //set arg
        lzw_kernel.setArg(0, lzw_chunks_buf);
        lzw_kernel.setArg(1, lzw_chunks_length_buf);
        lzw_kernel.setArg(2, lzw_file_buffer_buf);
        lzw_kernel.setArg(3, lzw_total_bytes_buf);
	
	for (unsigned int i = 0; i < chunk_count; i++) {
		if (dup_flag[i] == 0) {
            //memcpy data
            memcpy(&lzw_chunks, chunks[i], chunk_sizes[i]);
            lzw_chunks_length = (int*)&chunk_sizes[i];


            std::vector<cl::Event> write_events;
			std::vector<cl::Event> exec_events;

            cl::Event write_ev;
			cl::Event exec_ev;
			cl::Event read_ev;

			q.enqueueMigrateMemObjects({lzw_chunks_buf, lzw_chunks_length_buf}, 0 /* 0 means from host*/, NULL, &write_ev);
			write_events.push_back(write_ev); 	

			printf("enqueue task\n");
			q.enqueueTask(lzw_kernel, &write_events, &exec_ev);
			exec_events.push_back(exec_ev);

			printf("Memory object migration enqueue host->device\n");
			q.enqueueMigrateMemObjects({lzw_file_buffer_buf, lzw_total_bytes_buf}, CL_MIGRATE_MEM_OBJECT_HOST, &exec_events, &read_ev);
			q.finish();

			int output_index = convert_output(lzw_file_buffer, lzw_compressed_output[i], *lzw_total_bytes);
			compressed_data_size[i] = output_index;


		}
	}


        lzw_timer.stop();

    	// build header
	uint32_t header[chunk_count];
	for (unsigned int i = 0; i < chunk_count; i++) {

		header[i] = 0;
		if (dup_flag[i] == 0) {
			header[i] = compressed_data_size[i] << 1;
		} else {
			header[i] = (dup_index[i]<<1) | 0x00000001;
		}
	}




	//write in output bin
	FILE* out_file = fopen("compressed_data.bin", "wb");
	if (file == NULL) {
        perror("Failed to open file");
    }

    for (unsigned int i = 0; i < chunk_count; i++) {
		fwrite(&header[i], sizeof(uint32_t), 1, out_file);
		printf("%#010x", header[i]);
		if (dup_flag[i] == 0) {
			for (int j = 0; j < compressed_data_size[i]; j++) {
				fwrite(&lzw_compressed_output[i][j], sizeof(uint8_t), 1, out_file);
				printf("%02X ", lzw_compressed_output[i][j]);
			}
		}
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

	std::cout << "--------------- Key Throughputs ---------------" << std::endl;
	float ethernet_latency = ethernet_timer.latency() / 1000.0;
	float input_throughput = (bytes_written * 8 / 1000000.0) / ethernet_latency; // Mb/s
	std::cout << "Input Throughput to Encoder: " << input_throughput << " Mb/s."
			<< " (Latency: " << ethernet_latency << "s)." << std::endl;

	float cdc_latency = cdc_timer.latency() / 1000.0;
	float cdc_throughput = (bytes_written * 8 / 1000000.0) / cdc_latency; // Mb/s
	std::cout << "cdc Throughput to Encoder: " << cdc_throughput << " Mb/s."
			<< " (Latency: " << cdc_latency << "s)." << std::endl;

	float sha_latency = sha_timer.latency() / 1000.0;
	float sha_throughput = (bytes_written * 8 / 1000000.0) / sha_latency; // Mb/s
	std::cout << "sha Throughput to Encoder: " << sha_throughput << " Mb/s."
			<< " (Latency: " << sha_latency << "s)." << std::endl;

	float dedup_latency = dedup_timer.latency() / 1000.0;
	//float dedup_throughput = (dedup_chars * 8 / 1000000.0) / dedup_latency; // Mb/s
	//std::cout << "dedup Throughput to Encoder: " << dedup_throughput << " Mb/s."
	//		<< " (Latency: " << dedup_latency << "s)." << std::endl;

	float output_latency = overall_timer.latency() / 1000.0;
	//float output_throughput = (total_inputBits * 8 / 1000000.0) / output_latency; // Mb/s
	//std::cout << "output Throughput to Encoder: " << output_throughput << " Mb/s."
	//		<< " (Latency: " << output_latency << "s)." << std::endl;

// ------------------------------------------------------------------------------------
// Step 4: Check Results and Release Allocated Resources
// ------------------------------------------------------------------------------------
    // multiply_gold(in1, in2, out_sw);
    // bool match = Compare_matrices(out_sw, out_hw);
    // Destroy_matrix(out_sw);
    delete[] fileBuf;
	q.enqueueUnmapMemObject(lzw_chunks_buf, lzw_chunks);
	q.enqueueUnmapMemObject(lzw_chunks_length_buf, lzw_chunks_length);
	q.enqueueUnmapMemObject(lzw_file_buffer_buf, lzw_file_buffer);
	q.enqueueUnmapMemObject(lzw_total_bytes_buf, lzw_total_bytes);
    q.finish();

    // std::cout << "--------------- Key execution times ---------------" << std::endl;
    // timer.print();

    // std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl; 
    // return (match ? EXIT_SUCCESS : EXIT_FAILURE);
    return 0;
}
