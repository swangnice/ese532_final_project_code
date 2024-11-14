#ifndef LZW_H
#define LZW_H

#include <vector>
#include <unordered_map>
#include <string>

// 前置声明，以便在其他类和函数中使用
//class assoc_mem;

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

std::vector<int> encoding(const unsigned char* data, size_t size);

// // 定义LZW压缩算法类
// class LZWCompressor {
// public:
//     LZWCompressor();
//     std::vector<int> encode(const unsigned char* data, size_t size);

// private:
//     unsigned long* hash_table;    // 指向哈希表的指针
//     assoc_mem* associative_memory;  // 指向关联存储器的指针
//     std::unordered_map<std::string, int> table;  // 字典表

//     void initializeTable();
// };

#endif // LZW_H
