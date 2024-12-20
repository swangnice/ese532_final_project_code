#include "encoder.h"
#include "chunk.h"
#include "cdc.h"
#include "sha.h"
#include "../Server/lzw.h"
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
#include <sys/stat.h>
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
	//printf("buffer------------------------------: %p\n", buffer);
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
    unsigned int estimated_chunks = 4096;
    chunks = (unsigned char **)malloc(sizeof(unsigned char *) * estimated_chunks);
    chunk_sizes = (int *)malloc(sizeof(unsigned int) * estimated_chunks);


	cdc_timer.start();
	//rabin_fingerprint_cdc(&buffer[HEADER], length, &chunks, &chunk_count, &chunk_sizes);
	gear_based_fastcdc(&buffer[HEADER], length, &chunks, &chunk_count, &chunk_sizes);
	cdc_timer.stop();



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

	sha_timer.start();
	uint8_t sha256_output[chunk_count][32];

	for (unsigned int i = 0; i < chunk_count; i++) {
		unsigned char *temp_chunk_data = chunks[i];
    	unsigned int temp_chunk_size = chunk_sizes[i];

		unsigned char compressed_data[SHA_BLOCKSIZE];
    	int compressed_size = rle_compress((const unsigned char*)temp_chunk_data, temp_chunk_size, compressed_data, SHA_BLOCKSIZE);

		uint8_t temp_sha256_output[32]; 
		calculate_sha256(compressed_data, compressed_size, temp_sha256_output);

		// std::cout << "SHA-256 Hash of Compressed Data: ";
		// for (int i = 0; i < 32; i++) {
		// 	printf("%02x", temp_sha256_output[i]);
		// }
		// std::cout << std::endl;


		memcpy(sha256_output[i], temp_sha256_output, 32);
		
	}
	sha_timer.stop();
	
	//deduplication
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




	//lzw_timer.start();
	// uint8_t lzw_compressed_output[undup_count][2048];
	// int compressed_data_size[undup_count];

	// for (unsigned int i = 0; i < chunk_count; i++) {
	// 	if (dup_flag[i] == 0) {
	// 		int temp_output_index = 0;
	// 		uint16_t temp_lzw_compressed_output[chunk_sizes[i]];

	// 		lzw_compress(chunks[i], &chunk_sizes[i], temp_lzw_compressed_output, &temp_output_index);
	// 		int output_index = convert_output(temp_lzw_compressed_output, lzw_compressed_output[i], temp_output_index);
	// 		compressed_data_size[i] = output_index;
	// 	}
	// }

	for (unsigned int i = 0; i < chunk_count; i++) {
		if (dup_flag[i] == 0) {
			printf("Chunk %u:", i);
			printf("chunk size: %d\n", chunk_sizes[i]);
		}
	}

	//LZW Software
	// uint16_t temp_lzw_compressed_output[chunk_count][2048]; // 假设 MAX_CHUNK_SIZE 是单个块的最大大小
	// int temp_output_index[chunk_count];
	// uint8_t lzw_compressed_output[undup_count][2048];
	// int compressed_data_size[undup_count];
    uint8_t *out_buffer;
    out_buffer = (uint8_t *)malloc(sizeof(uint8_t) * 1024 * 512); //    512KB Here, we assume that the maximum size of the whole compressed data is 512KB
    //printf("Initial out_buffer size: %d\n", sizeof(out_buffer));
    uint8_t *temp_out_buffer;
    temp_out_buffer = (uint8_t *)malloc(sizeof(uint8_t) * 1024 * 2); //   2KB Here, we assume that the maximum size of the compressed data of one chunk is 2KB
    //printf("Initial temp_out_buffer size: %d\n", sizeof(temp_out_buffer));
    unsigned int temp_out_buffer_size = 0;
    size_t out_offset = 0;
    for (unsigned int i = 0; i < chunk_count; i++) {
        //memset(temp_out_buffer, 0, 1024 * 2);
        lzw_compress_v2(chunks[i], &chunk_sizes[i], dup_flag[i], dup_index[i], temp_out_buffer, &temp_out_buffer_size);
        //printf("%s\n", temp_out_buffer);
        memcpy(out_buffer + out_offset, temp_out_buffer, temp_out_buffer_size);
        out_offset += temp_out_buffer_size;
    }
    printf("Final out_buffer size: %d\n", sizeof(out_buffer));

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

	// // lzw compress
	// for (unsigned int i = 0; i < chunk_count; i++) {
	// 	if (dup_flag[i] == 0) {
	// 		lzw_compress(chunks[i], &chunk_sizes[i], temp_lzw_compressed_output[i], &temp_output_index[i]);
	// 	}
	// }

	// // convert output
	// for (unsigned int i = 0; i < chunk_count; i++) {
	// 	if (dup_flag[i] == 0) {
	// 		printf("Chunk %u:", i);
	// 		printf("temp_output_index: %d\n", temp_output_index[i]);
	// 	}
	// }
	// for (unsigned int i = 0; i < chunk_count; i++) {
	// 	if (dup_flag[i] == 0) {
	// 		int output_index = convert_output(temp_lzw_compressed_output[i], lzw_compressed_output[i], temp_output_index[i]);
	// 		compressed_data_size[i] = output_index;
	// 	}
	// }


	// // for (unsigned int i = 0; i < chunk_count; i++) {
	// // 	if (dup_flag[i] == 0) {
	// // 		printf("Chunk %u: ", i);
	// // 		for (int j = 0;j < compressed_data_size[i]; j++) {
	// // 			printf("%02X ", lzw_compressed_output[i][j]);
	// // 		}
	// // 		printf("\n");
	// // 	}
    // // }
	// lzw_timer.stop();

	
	// // build header
	// uint32_t header[chunk_count];
	// for (unsigned int i = 0; i < chunk_count; i++) {

	// 	header[i] = 0;
	// 	if (dup_flag[i] == 0) { // not duplicated
	// 		header[i] = compressed_data_size[i] << 1;
	// 	} else {
	// 		header[i] = (dup_index[i]<<1) | 0x00000001;
	// 	}
	// 	//header[i] = (temp_header << 24) & 0xff000000 | (temp_header<<8)&0x00ff0000 | (temp_header>>8)&0x0000ff00 | (temp_header>>24)&0x000000ff;

	// 	//printf("chunk: %u, Header: %#010x\n", i, header[i]);
	// 	//printf("chunk: %u, Header: %#010x\n", i, temp_header);
	// }

	// //overall time
	// overall_timer.stop();

	// //write in output bin
	// FILE* out_file = fopen("compressed_data.bin", "wb");
	// if (file == NULL) {
    //     perror("Failed to open file");
        
    // }
	// for (unsigned int i = 0; i < chunk_count; i++) {
	// 	fwrite(&header[i], sizeof(uint32_t), 1, out_file);
	// 	printf("\n %#010x ", header[i]);
	// 	if (dup_flag[i] == 0) {
	// 		for (int j = 0; j < compressed_data_size[i]; j++) {
	// 			fwrite(&lzw_compressed_output[i][j], sizeof(uint8_t), 1, out_file);
	// 			printf("%02X", lzw_compressed_output[i][j]);
	// 		}
	// 	}
	// }
	// fclose(out_file);

	overall_timer.stop();

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
