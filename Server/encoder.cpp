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

	// SHA-256 hash calculation
	unsigned char **sha256_output = NULL;
	sha256_output = (unsigned char **)malloc(sizeof(unsigned char *) * chunk_count);
	if (sha256_output == NULL) {
		printf("Failed to allocate memory for sha256_output.\n");
		return 1;
	}

	// 32byte for each chunk
	for (unsigned int i = 0; i < chunk_count; i++) {
		sha256_output[i] = (unsigned char *)malloc(32 * sizeof(unsigned char));
		if (sha256_output[i] == NULL) {
			printf("Failed to allocate memory for sha256_output[%u].\n", i);
			for (unsigned int j = 0; j < i; j++) {
				free(sha256_output[j]);
			}
			free(sha256_output);
			return 1;
		}
	}

	for (unsigned int i = 0; i < chunk_count; i++) {
		unsigned char *temp_chunk_data = chunks[i];
    	unsigned int temp_chunk_size = chunk_sizes[i];

		unsigned char compressed_data[SHA_BLOCKSIZE];
    	int compressed_size = rle_compress((const unsigned char*)temp_chunk_data, temp_chunk_size, compressed_data, SHA_BLOCKSIZE);

		std::cout << "Original Size: " << temp_chunk_size << " bytes\n";
		std::cout << "Compressed Size: " << compressed_size << " bytes\n";

		// 计算压缩数据的 SHA-256 哈希
		uint8_t temp_sha256_output[32]; // SHA-256 输出为 32 字节的 uint8_t 数组
		calculate_sha256(compressed_data, compressed_size, temp_sha256_output);

		// 输出 SHA-256 哈希结果

		memcpy(sha256_output[i], temp_sha256_output, 32);
		std::cout << "SHA-256 Hash of Compressed Data: ";
		for (int i = 0; i < 32; i++) {
			printf("%02x", temp_sha256_output[i]);
		}
		std::cout << std::endl;
		
	}
	
	//deduplication and assign lzw header
	uint32_t duplicate_flag[chunk_count];    //0-LZW 1-Dup
	uint32_t header[chunk_count];
	duplicate_flag[0] = 0;
	int index = 0;
	for (unsigned int i = 1; i < chunk_count; i++) {
		duplicate_flag[i] = 0;						//initialize all flags to 0
		for (unsigned int j = 0; j < i; j++) {
			if (chunk_sizes[i] == chunk_sizes[j]) {
				if (sha256_output[i] == sha256_output[j]) {
					duplicate_flag[i] = 1;
					header[i] = header[j] | 0xFFFFFFFE;
					break;
				}
			}
		}
		index++;
		header[i] = (index<<1) | duplicate_flag[i];
	}
	for (unsigned int i = 0; i < chunk_count; i++) {
		printf("Chunk %u: ", i);
		printf("Header: %#010x\n", header[i]);
	}




	// write file to root and you can use diff tool on board
	

	for (int i = 0; i < NUM_PACKETS; i++) {
		free(input[i]);
	}

	for (unsigned int i = 0; i < chunk_count; ++i) {
        free(chunks[i]);
    }
    free(chunks);
    free(chunk_sizes);
    //free(buffer);
	for (unsigned int i = 0; i < chunk_count; i++) {
		free(sha256_output[i]);
	}
	free(sha256_output);

	free(file);
	std::cout << "--------------- Key Throughputs ---------------" << std::endl;
	float ethernet_latency = ethernet_timer.latency() / 1000.0;
	float input_throughput = (bytes_written * 8 / 1000000.0) / ethernet_latency; // Mb/s
	std::cout << "Input Throughput to Encoder: " << input_throughput << " Mb/s."
			<< " (Latency: " << ethernet_latency << "s)." << std::endl;

	return 0;
}
