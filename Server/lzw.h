#ifndef LZW_COMPRESS_H
#define LZW_COMPRESS_H

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


typedef struct
{   
    // Each key_mem has a 9 bit address (so capacity = 2^9 = 512)
    // and the key is 20 bits, so we need to use 3 key_mems to cover all the key bits.
    // The output width of each of these memories is 64 bits, so we can only store 64 key
    // value pairs in our associative memory map.

    unsigned long upper_key_mem[512]; // the output of these  will be 64 bits wide (size of unsigned long).
    unsigned long middle_key_mem[512];
    unsigned long lower_key_mem[512]; 
    unsigned int value[64];    // value store is 64 deep, because the lookup mems are 64 bits wide
    unsigned int fill;         // tells us how many entries we've currently stored 
} assoc_mem;

// 定义哈希表操作和关联存储器操作的相关函数
void hash_insert(unsigned long* hash_table, unsigned int key, unsigned int value, bool* collision);
void hash_lookup(unsigned long* hash_table, unsigned int key, bool* hit, unsigned int* result);

void assoc_insert(assoc_mem* mem, unsigned int key, unsigned int value, bool* collision);
void assoc_lookup(assoc_mem* mem, unsigned int key, bool* hit, unsigned int* result);

void insert(unsigned long* hash_table, assoc_mem* mem, unsigned int key, unsigned int value, bool* collision);
void lookup(unsigned long* hash_table, assoc_mem* mem, unsigned int key, bool* hit, unsigned int* result);

void lzw_compress(unsigned char* s1, int* length, uint16_t* out_code, int *out_len);
int convert_output(uint16_t in[], uint8_t out[], int input_size);

void lzw_compress_v2(unsigned char* s1, int* length, int *is_dup, int *dup_index,  uint8_t *temp_out_buffer, unsigned int *temp_out_buffer_size);
#ifdef __cplusplus
extern "C" {
#endif
void lzw_compress_hw(unsigned char* s1, int* length, int *is_dup, int *dup_index,  uint8_t *temp_out_buffer, unsigned int *temp_out_buffer_size);

#ifdef __cplusplus
}
#endif
#endif // LZW_COMPRESS_H 
