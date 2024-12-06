#include "cdc.h"

uint64_t initial_hash_func(const unsigned char *input, unsigned int pos) {
    uint64_t hash = 0;
    for (int i = 0; i < WIN_SIZE; ++i) {
        hash += input[pos + WIN_SIZE - 1 - i] * (uint64_t)pow(PRIME, i + 1);
    }
    return hash;
}

uint64_t rolling_hash(uint64_t previous_hash, unsigned char old_char, unsigned char new_char) {
    return (previous_hash * PRIME) - (old_char * (uint64_t)pow(PRIME, WIN_SIZE + 1)) + (new_char * PRIME);
}

// CDC function to split file into chunks based on hash condition
void rabin_fingerprint_cdc(const unsigned char *buff, unsigned int buff_size, unsigned char ***chunks, unsigned int *chunk_count, int **chunk_sizes) {
    if (buff_size < WIN_SIZE) return;
    // Calculate initial hash for the first window
    uint64_t current_hash = initial_hash_func(buff, 0);
    unsigned int start = 0;

    for (unsigned int i = WIN_SIZE; i <= buff_size; ++i) {
        if (i < buff_size) {
            current_hash = rolling_hash(current_hash, buff[i - WIN_SIZE], buff[i]);
        }

        // Check if the hash meets the target condition or it's the last byte
        if ((current_hash % MODULUS == TARGET1) || (current_hash % MODULUS == TARGET2) || (i == buff_size)) {
            unsigned int chunk_size = i - start;
            (*chunks)[*chunk_count] = (unsigned char *)malloc(chunk_size);
            memcpy((*chunks)[*chunk_count], buff + start, chunk_size);
            (*chunk_sizes)[*chunk_count] = chunk_size;
            (*chunk_count)++;
            start = i;
        }
    }
}
