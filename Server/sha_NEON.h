#ifndef SHA_H
#define SHA_H

#include <stdint.h>
#include <arm_neon.h>

#define SHA256_BLOCK_SIZE 32 // SHA256 outputs a 32 byte digest


void calculate_sha256(const unsigned char* data, int data_len, uint8_t hash[32]);

#endif // SHA_H