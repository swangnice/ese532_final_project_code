#include "encoder.h"
#include "chunk.h"
#include "cdc.h"
#include "sha.h"
#include "lzw.h"
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
    unsigned int *chunk_sizes = NULL;
	chunk_count = 0;
    unsigned int estimated_chunks = 15360 / WIN_SIZE + 1;
    chunks = (unsigned char **)malloc(sizeof(unsigned char *) * estimated_chunks);
    chunk_sizes = (unsigned int *)malloc(sizeof(unsigned int) * estimated_chunks);
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
	// unsigned char **sha256_output = NULL;
	// sha256_output = (unsigned char **)malloc(sizeof(unsigned char *) * chunk_count);
	// if (sha256_output == NULL) {
	// 	printf("Failed to allocate memory for sha256_output.\n");
	// 	return 1;
	// }

	// // 32byte for each chunk
	// for (unsigned int i = 0; i < chunk_count; i++) {
	// 	sha256_output[i] = (unsigned char *)malloc(32 * sizeof(unsigned char));
	// 	if (sha256_output[i] == NULL) {
	// 		printf("Failed to allocate memory for sha256_output[%u].\n", i);
	// 		for (unsigned int j = 0; j < i; j++) {
	// 			free(sha256_output[j]);
	// 		}
	// 		free(sha256_output);
	// 		return 1;
	// 	}
	// }
	sha_timer.start();
	uint8_t sha256_output[chunk_count][32];

	for (unsigned int i = 0; i < chunk_count; i++) {
		unsigned char *temp_chunk_data = chunks[i];
    	unsigned int temp_chunk_size = chunk_sizes[i];

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
	int dict[MAX_DICT_SIZE][256];

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
	for (unsigned int i = 0; i < chunk_count; i++) {
		if (dup_flag[i] == 0) {
			int temp_output_index = 0;
			
			uint16_t temp_lzw_compressed_output[chunk_sizes[i]];



			std::string temp_chunk = reinterpret_cast<const char*>(chunks[i]);
			const unsigned char* temp_data = reinterpret_cast<const unsigned char*>(temp_chunk.c_str());
			std::vector<int> output_code = encoding(temp_data, chunk_sizes[i]);
			compressed_data_size[i] = output_code.size()*sizeof(int);
			int index = 0;
			for (int output : output_code) {
				lzw_compressed_output[i][index] = ((output >> 24) & 0xFF);
				index++;
				lzw_compressed_output[i][index] = ((output >> 16) & 0xFF);
				index++;
				lzw_compressed_output[i][index] = ((output >> 8) & 0xFF);
				index++;
				lzw_compressed_output[i][index] = (output & 0xFF);
				index++;
			}


			//lzw(chunks[i], chunk_sizes[i], dict, temp_lzw_compressed_output, temp_output_index);
			//int output_index = convert_output(temp_lzw_compressed_output, lzw_compressed_output[i], temp_output_index);
			//compressed_data_size[i] = output_index;
		}
	}
	// for (unsigned int i = 0; i < chunk_count; i++) {
	// 	if (dup_flag[i] == 0) {
	// 		printf("Chunk %u: ", i);
	// 		for (int j = 0;j < compressed_data_size[i]; j++) {
	// 			printf("%02X ", lzw_compressed_output[i][j]);
	// 		}
	// 		printf("\n");
	// 	}
    // }
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
		//header[i] = (temp_header << 24) & 0xff000000 | (temp_header<<8)&0x00ff0000 | (temp_header>>8)&0x0000ff00 | (temp_header>>24)&0x000000ff;

		//printf("chunk: %u, Header: %#010x\n", i, header[i]);
		//printf("chunk: %u, Header: %#010x\n", i, temp_header);
	}
	// for (unsigned int i = 0; i < chunk_count; i++) {
	// 	//header[i] = ((header[i] << 24) & 0xff000000) | ((header[i]<<8)&0x00ff0000) | ((header[i]>>8)&0x0000ff00) | ((header[i]>>24)&0x000000ff);
	// 	printf("chunk: %u, Header: %#010x\n", i, header[i]);

	// }

	// for (unsigned int i = 0; i < chunk_count; i++) {
	// 	printf("chunk: %u, original size:%d, duplicated: %d, duplicated with %d compressed sized: %d\n", i, chunk_sizes[i], dup_flag[i], dup_index[i], compressed_data_size[dup_index[i]]);
	// }

	//overall time
	overall_timer.stop();
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
    //free(buffer);
	// for (unsigned int i = 0; i < chunk_count; i++) {
	// 	free(sha256_output[i]);
	// }
	// free(sha256_output);

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
