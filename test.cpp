#include <iostream>
#include <vector>
#include <unordered_map>
#include <stdlib.h>
#include "Server/lzw.h"


int main() {

	unsigned char s[] = {'W', 'Y', 'S', '*', 'W', 'Y', 'G', 'W', 'Y', 'S', '*', 'W', 'Y', 'S', 'W', 'Y', 'S', 'G'};
	int length_value = 18;

	int is_dup = 0;
	int dup_index = 0;


	uint8_t *temp_out_buffer_hw = (uint8_t*)malloc(sizeof(uint8_t) * 2048);
	uint8_t *temp_out_buffer_v2 = (uint8_t*)malloc(sizeof(uint8_t) * 2048);
	unsigned int *temp_out_buffer_size_v2 = (unsigned int*)malloc(sizeof(unsigned int));
	unsigned int *temp_out_buffer_size_hw = (unsigned int*)malloc(sizeof(unsigned int));

	lzw_compress_hw(s, &length_value, &is_dup, &dup_index, temp_out_buffer_hw, temp_out_buffer_size_hw);
	lzw_compress_v2(s, &length_value, &is_dup, &dup_index, temp_out_buffer_v2, temp_out_buffer_size_v2);

// 确保两个缓冲区大小一致
	if (*temp_out_buffer_size_hw != *temp_out_buffer_size_v2) {
		return 1; // 大小不一致，返回错误
	}

	// 比较缓冲区内容
	for (int i = 0; i < *temp_out_buffer_size_hw; i++) {
		if (temp_out_buffer_hw[i] != temp_out_buffer_v2[i]) {
			return 1; // 内容不匹配，返回错误
		}
	}


	return 0;

}
