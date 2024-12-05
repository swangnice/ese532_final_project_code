#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define WIN_SIZE 16    // Window size for hashing
#define PRIME 3      // Prime number for hash calculation
#define MODULUS 512  // Modulus for chunk split condition
#define TARGET1 55    // Target hash value for chunk splitting
#define TARGET2 66    // Target hash value for chunk splitting

uint64_t initial_hash_func(const unsigned char *input, unsigned int pos);
uint64_t rolling_hash(uint64_t previous_hash, unsigned char old_char, unsigned char new_char);
void cdc(const unsigned char *buff, unsigned int buff_size, unsigned char ***chunks, unsigned int *chunk_count, int **chunk_sizes);
