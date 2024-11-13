#include "encoder.h"
#include "chunk.h"
#include "cdc.h"
#include "sha.h"
#include "lzw_encode.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "server.h"
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "stopwatch.h"

#define NUM_PACKETS 8
#define pipe_depth 4
#define DONE_BIT_L (1 << 7)
#define DONE_BIT_H (1 << 15)

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

int main(int argc, char* argv[]) {
	stopwatch ethernet_timer;
	unsigned char* input[NUM_PACKETS];
	int writer = 0;
	int done = 0;
	int length = 0;
	int count = 0;
	ESE532_Server server;

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
	memcpy(&file[offset], &buffer[HEADER], length);
	//printf("%.*s\n", length, &buffer[HEADER]);
	unsigned char **chunks = NULL;
    unsigned int chunk_count = 0;
    unsigned int *chunk_sizes = NULL;
	chunk_count = 0;
    unsigned int estimated_chunks = 15360 / WIN_SIZE + 1;
    chunks = (unsigned char **)malloc(sizeof(unsigned char *) * estimated_chunks);
    chunk_sizes = (unsigned int *)malloc(sizeof(unsigned int) * estimated_chunks);
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

	// FILE *outfd = fopen("output_cpu.bin", "wb");
	// int bytes_written = fwrite(&file[0], 1, offset, outfd);
	// printf("write file with %d\n", bytes_written);
	// fclose(outfd);
	
	int dict[MAX_DICT_SIZE][256];

	init_dict(dict);

	uint8_t dict_isdup[32 * chunk_count];
	memset(dict_isdup, 0, 32 * chunk_count);
	int index = 0;//indicate last part of the dict_isdup
	int out_index = 0;//indictae output index
	unsigned char* buffer_out = (unsigned char*) malloc(sizeof(unsigned char) * 70000000);

	for (unsigned int i = 0; i < chunk_count; i++) {
		unsigned char *temp_chunk_data = chunks[i];
    	unsigned int temp_chunk_size = chunk_sizes[i];

		unsigned char compressed_data[SHA_BLOCKSIZE];
    	int compressed_size = rle_compress((const unsigned char*)temp_chunk_data, temp_chunk_size, compressed_data, SHA_BLOCKSIZE);

		// calculate hash
		uint8_t temp_sha256_output[32]; // SHA-256 输出�? 32 字节�? uint8_t 数组
		calculate_sha256(compressed_data, compressed_size, temp_sha256_output);

		int isdup = 0;

		// dedup module
		// determine if it is dup
		for(int j = 0; j < index; ++j){
			int flag = 0;
			for(int k = 0; k < 32; ++k){
				if(dict_isdup[j * 32 + k] == temp_sha256_output[k]){
					flag++;
				}
			}
			//uint8_t header[4];

			//this is dup
			if(flag == 32){
				//send dup header
				isdup = 1;
				//header for dup
				buffer_out[out_index++] = ((j & 0x7f) << 1);
				buffer_out[out_index++] = j >> 7;
				buffer_out[out_index++] = j >> 15;
				buffer_out[out_index++] = j >> 23;
			}
		}

		int output_index = 0;

		//this is not dup
		if(!isdup){
			for(int j = 0; j < 32; ++j){
				dict_isdup[index * 32 + j] = temp_sha256_output[j];
			}
			index++;
			uint16_t temp_output[compressed_size];
			lzw(compressed_data, compressed_size, dict, temp_output, output_index);
			convert_output(temp_output, buffer_out, output_index, out_index);
		}
		
	}
	
	FILE *outfd = fopen("output.bin", "wb");
	size_t bytes_written = fwrite(buffer_out, 1, out_index, outfd);
	fclose(outfd);
	// write file to root and you can use diff tool on board
	
	free(buffer_out);

	for (int i = 0; i < NUM_PACKETS; i++) {
		free(input[i]);
	}

	for (unsigned int i = 0; i < chunk_count; ++i) {
        free(chunks[i]);
    }
    free(chunks);
    free(chunk_sizes);

	free(file);
	std::cout << "--------------- Key Throughputs ---------------" << std::endl;
	float ethernet_latency = ethernet_timer.latency() / 1000.0;
	float input_throughput = (bytes_written * 8 / 1000000.0) / ethernet_latency; // Mb/s
	std::cout << "Input Throughput to Encoder: " << input_throughput << " Mb/s."
			<< " (Latency: " << ethernet_latency << "s)." << std::endl;

	return 0;
}
