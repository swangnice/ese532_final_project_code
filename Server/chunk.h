#ifndef CHUNK_H
#define CHUNK_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    unduplicated = 0,
    duplicated = 1,
    uncompared = 2
} DuplicateType;

typedef struct ChunkNode {
    char *key;
    int value;
    int32_t chunk_index;
    uint8_t *text;  // The content of the chunk
    DuplicateType isDuplicated;
} ChunkNode;

typedef struct ChunkTable {
    ChunkNode **buckets;
} ChunkTable;

ChunkTable* createChunkTable();
void addChunk(ChunkTable* table, unsigned char* text, size_t length, int index);
void freeChunkTable(ChunkTable* table);
#endif