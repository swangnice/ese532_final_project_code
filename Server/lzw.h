#ifndef LZW_H
#define LZW_H

#include <vector>
#include <unordered_map>
#include <string>

// 前置声明，以便在其他类和函数中使用
class assoc_mem;

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
