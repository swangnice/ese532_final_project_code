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

inline uint64_t gear_rolling_hash(uint64_t h, uint8_t c){
    return ((h << 1) ^ gear_table[c]);
}

// Gear based CDC function to split file into chunks
void gear_based_fastcdc(const unsigned char *buff, unsigned int buff_size, unsigned char ***chunks, unsigned int *chunk_count, int **chunk_sizes) {

    unsigned int start = 0;
    unsigned int i = 0;
    uint64_t current_hash = 0;
    uint64_t mask_small = FASTCDC_MASK_S; //#define FASTCDC_MASK_S 0x59370353;  //15 1
    uint64_t mask_large = FASTCDC_MASK_L; //#define FASTCDC_MASK_L 0xd9000353;  //13 1

    if(buff_size < FASTCDC_MIN_CHUNK) {
        return;
    }

    unsigned int chunk_size = 0;
    while (i < buff_size) {
        current_hash = gear_rolling_hash(current_hash, buff[i]);
        if ((chunk_size < FASTCDC_AVG_CHUNK && ((current_hash & mask_small) == 0)) /*small chunk*/|| (chunk_size >= FASTCDC_AVG_CHUNK && ((current_hash & mask_large) == 0)) /*large chunk*/|| (chunk_size == FASTCDC_MAX_CHUNK - 1)/*Max Chunk */|| (i == buff_size-1)/*remain part*/) {
            //unsigned int chunk_size = i - start + 1;
            if (chunk_size >= FASTCDC_MIN_CHUNK) {
                (*chunks)[*chunk_count] = (unsigned char *)malloc(chunk_size);
                memcpy((*chunks)[*chunk_count], buff + start, chunk_size);
                (*chunk_sizes)[*chunk_count] = chunk_size;
                (*chunk_count)++;
                start = i + 1;
                buff_size = 0;
            }
        }
        i++;
        chunk_size++;
    }
}