#include <iostream>
#include <vector>
#include <unordered_map>
#include <stdlib.h>
#include "Server/lzw.h"


int main() {

	unsigned char *s = (unsigned char *)"ksfkaghjsdnfjxfashcoas";

	int length_value = sizeof(s);

	int is_dup = 0;
	int dup_index = 22;

	uint8_t *temp_out_buffer_v2 = (uint8_t*)malloc(sizeof(uint8_t) * 2048);
	uint8_t *temp_out_buffer_hw = (uint8_t*)malloc(sizeof(uint8_t) * 2048);
	unsigned int *temp_out_buffer_size_v2 = (unsigned int*)malloc(sizeof(unsigned int));
	unsigned int *temp_out_buffer_size_hw = (unsigned int*)malloc(sizeof(unsigned int));

	lzw_compress_hw(s, &length_value, &is_dup, &dup_index, temp_out_buffer_hw, temp_out_buffer_size_hw);
	lzw_compress_v2(s, &length_value, &is_dup, &dup_index, temp_out_buffer_v2, temp_out_buffer_size_v2);

	if (*temp_out_buffer_size_hw != *temp_out_buffer_size_v2) {
		return 1;
	}

	for (int i = 0; i < *temp_out_buffer_size_hw; i++) {
		if (temp_out_buffer_hw[i] != temp_out_buffer_v2[i]) {
			printf("%02X,SIGSEGV %02X \n", temp_out_buffer_hw[i], temp_out_buffer_v2[i]);
			return 1;
		}
	}
	return 0;
}
