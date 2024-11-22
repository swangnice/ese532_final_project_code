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
    
    cl::Kernel lzw_kernel = cl::Kernel(program,"lzw_compress", &err);
    
// ------------------------------------------------------------------------------------
// Step 2: Create buffers and initialize test values
// ------------------------------------------------------------------------------------

    unsigned char* lzw_s1;
    int *lzw_length;
    uint16_t* lzw_out_code;
    int *lzw_out_len;

    cl::Buffer lzw_s1_buf = cl::Buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY,  sizeof(unsigned char) * CHUNKS_IN_SINGLE_KERNEL * MAX_CHUNK_SIZE, NULL, &err);
    cl::Buffer lzw_length_buf = cl::Buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY, sizeof(int) * CHUNKS_IN_SINGLE_KERNEL, NULL, &err);
    cl::Buffer lzw_out_code_buf = cl::Buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY,  sizeof(uint16_t) * CHUNKS_IN_SINGLE_KERNEL * MAX_FILE_BUFFER_SIZE, NULL, &err);
    cl::Buffer lzw_out_len_buf = cl::Buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY,  sizeof(int) * CHUNKS_IN_SINGLE_KERNEL, NULL, &err);
	
    lzw_s1 = (unsigned char *)q.enqueueMapBuffer(lzw_s1_buf, CL_TRUE, CL_MAP_WRITE, 0, sizeof(unsigned char) * CHUNKS_IN_SINGLE_KERNEL * MAX_CHUNK_SIZE);
    lzw_length = (int*)q.enqueueMapBuffer(lzw_length_buf, CL_TRUE, CL_MAP_WRITE, 0, sizeof(int) * CHUNKS_IN_SINGLE_KERNEL);
    lzw_out_code = (uint16_t*)q.enqueueMapBuffer(lzw_out_code_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(uint16_t) * CHUNKS_IN_SINGLE_KERNEL * MAX_FILE_BUFFER_SIZE);
    lzw_out_len = (int*)q.enqueueMapBuffer(lzw_out_len_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(int) * CHUNKS_IN_SINGLE_KERNEL);
	
// ------------------------------------------------------------------------------------
// Step 3: Run the kernel
// ------------------------------------------------------------------------------------
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
	server.get_packet(input[writer]);
	count++;

	// get packet
	unsigned char* buffer = input[writer];

	done = buffer[1] & DONE_BIT_L;
	length = buffer[0] | (buffer[1] << 8);
	length &= ~DONE_BIT_H;

	//overall_timer.start();
	memcpy(&file[offset], &buffer[HEADER], length);

    unsigned int estimated_chunks = 15360 / WIN_SIZE + 1;

	unsigned char **chunks = NULL;
    unsigned int chunk_count = 0;
    int *chunk_sizes = NULL;
    chunks = (unsigned char **)malloc(sizeof(unsigned char *) * estimated_chunks);
	chunk_count = 0;
    chunk_sizes = (int *)malloc(sizeof(int) * estimated_chunks);
	//cdc_timer.start();
	cdc(&buffer[HEADER], length, &chunks, &chunk_count, &chunk_sizes);
	//cdc_timer.stop();
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
		//cdc_timer.start();
		cdc(&buffer[HEADER], length, &chunks, &chunk_count, &chunk_sizes);
		//cdc_timer.stop();
		
		offset += length;
		writer++;
	}

    FILE *outfd = fopen("output_cpu.bin", "wb");
	int bytes_written = fwrite(&file[0], 1, offset, outfd);
	printf("write file with %d\n", bytes_written);
	fclose(outfd);

	//print all chunks
	printf("Chunk count: %u\n", chunk_count);
	printf("Chunk sizes:\n");
	for (unsigned int i = 0; i < chunk_count; i++) {
		printf("Chunk %u size: %u bytes\n", i, chunk_sizes[i]);
	}
	printf("Chunk data (as string):\n");
	for (unsigned int i = 0; i < chunk_count; i++) {
		printf("Chunk %u: ", i);
		printf("%.*s\n", chunk_sizes[i], (char *)chunks[i]);
	}

    //SHA256 hash
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

    // Deduplication
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

    // LZW on HW
    //uint16_t temp_lzw_compressed_output[chunk_count][MAX_CHUNK_SIZE]; // 假设 MAX_CHUNK_SIZE 是单个块的最大大小
    //int temp_output_index[chunk_count];
    uint8_t lzw_compressed_output[undup_count][2048];
    int compressed_data_size[undup_count];

    for (unsigned int i = 0; i < chunk_count; i++) {
        memcpy(&lzw_s1[i * MAX_CHUNK_SIZE], chunks[i], chunk_sizes[i]);
        lzw_length[i] = chunk_sizes[i];
    }

    for (unsigned int i = 0; i < chunk_count; i++) {
        if (dup_flag[i] == 0) {
            //lzw_compress(chunks[i], &chunk_sizes[i], temp_lzw_compressed_output[i], &temp_output_index[i]);
            lzw_kernel.setArg(0, lzw_s1_buf);
            lzw_kernel.setArg(1, lzw_length_buf);
            lzw_kernel.setArg(2, lzw_out_code_buf);
            lzw_kernel.setArg(3, lzw_out_len_buf);

            std::vector<cl::Event> write_events;
            std::vector<cl::Event> exec_events;

            //cl::Event write_ev;
            //cl::Event exec_ev;
            //cl::Event read_ev;

            // q.enqueueMigrateMemObjects({lzw_s1_buf, lzw_length_buf}, 0 /* 0 means from host*/, NULL, &write_ev);
			// write_events.push_back(write_ev); 
            // q.enqueueTask(lzw_kernel, &write_events, &exec_ev);
			// exec_events.push_back(exec_ev);
            // q.enqueueMigrateMemObjects({lzw_out_code_buf, lzw_out_len_buf}, CL_MIGRATE_MEM_OBJECT_HOST, &exec_events, &read_ev);
            // q.finish();

            cl::Event write_ev;
            q.enqueueMigrateMemObjects({lzw_s1_buf, lzw_length_buf}, 0, NULL, &write_ev);

            cl::Event exec_ev;
            // Create a vector for the event dependency
            write_events.push_back(write_ev);
            q.enqueueTask(lzw_kernel, &write_events, &exec_ev);

            cl::Event read_ev;
            // Create another vector for the event dependency
            exec_events.push_back(exec_ev);
            q.enqueueMigrateMemObjects({lzw_out_code_buf, lzw_out_len_buf}, CL_MIGRATE_MEM_OBJECT_HOST, &exec_event_vec, &read_ev);

            // 等待完成
            q.finish();

        
        }
    }

    for (unsigned int i = 0; i < chunk_count; i++) {
        if (dup_flag[i] == 0) {
            int output_index = convert_output(&lzw_out_code[i], lzw_compressed_output[i], lzw_out_len[i]);
            compressed_data_size[i] = output_index;
        }
    }

    uint32_t header[chunk_count];
	for (unsigned int i = 0; i < chunk_count; i++) {

		header[i] = 0;
		if (dup_flag[i] == 0) {
			header[i] = compressed_data_size[i] << 1;
		} else {
			header[i] = (dup_index[i]<<1) | 0x00000001;
		}
		//header[i] = (temp_header << 24) & 0xff000000 | (temp_header<<8)&0x00ff0000 | (temp_header>>8)&0x0000ff00 | (temp_header>>24)&0x000000ff;

		//printf("chunk: %u, Header: %#010x\n", i, header[i]);
		//printf("chunk: %u, Header: %#010x\n", i, temp_header);
	}





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
    delete[] fileBuf;
    q.enqueueUnmapMemObject(lzw_s1_buf, lzw_s1);
    q.enqueueUnmapMemObject(lzw_length_buf, lzw_length);
    q.enqueueUnmapMemObject(lzw_out_code_buf, lzw_out_code);
    q.enqueueUnmapMemObject(lzw_out_len_buf, lzw_out_len);
    q.finish();

    return 0;

}
