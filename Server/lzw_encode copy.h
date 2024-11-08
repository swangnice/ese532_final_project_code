#ifndef LZW_ENCODE_H
#define LZW_ENCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_DICT_SIZE 4096 //local computer test can adjust value

int compare_match(int dict[MAX_DICT_SIZE][256], uint8_t curr_string[], int curr_string_len, int dict_index);

void lzw(uint8_t input[], int size, int dict[MAX_DICT_SIZE][256], uint16_t output[], int& output_index);

void init_dict(int dict[MAX_DICT_SIZE][256]);

void convert_output(uint16_t in[], uint8_t out[], int size);

#endif