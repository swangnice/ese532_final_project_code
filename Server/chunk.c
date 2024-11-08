#include <stdlib.h>
#include <string.h>
#include "chunk.h"


#define TABLE_SIZE 128

ChunkTable* createChunkTable() {
    ChunkTable* table = malloc(sizeof(ChunkTable));
    if (!table) return NULL;

    table->buckets = malloc(sizeof(ChunkNode*) * TABLE_SIZE);
    if (!table->buckets) {
        free(table);
        return NULL;
    }

    memset(table->buckets, 0, sizeof(ChunkNode*) * TABLE_SIZE);
    return table;
}


void addChunk(ChunkTable* table, unsigned char* text, size_t length, int index) {
    // Create a new chunk node
    ChunkNode* node = malloc(sizeof(ChunkNode));
    node->text = malloc(length);
    memcpy(node->text, text, length);  // Copy the chunk content
    node->chunk_index = index;
    node->isDuplicated = uncompared;

    // Hash function to find the right bucket; simple modulo for demonstration
    int bucket = index % TABLE_SIZE;

    // Handle collisions - here we use a simple linear probing
    while (table->buckets[bucket] != NULL) {
        bucket = (bucket + 1) % TABLE_SIZE;
    }

    table->buckets[bucket] = node;
}

void freeChunkTable(ChunkTable* table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (table->buckets[i] != NULL) {
            free(table->buckets[i]->text);
            free(table->buckets[i]);
        }
    }
    free(table->buckets);
    free(table);
}

