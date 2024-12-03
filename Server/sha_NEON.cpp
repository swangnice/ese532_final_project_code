#include "sha.h"
#include <string.h>

// SHA256 常量表
static const uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

// 内部辅助函数：处理一个 512-bit 块
static void sha256_transform_neon(const uint8_t data[64], uint32_t state[8]) {
    uint32x4_t abcd = vld1q_u32(&state[0]); // 前四个状态变量
    uint32x4_t efgh = vld1q_u32(&state[4]); // 后四个状态变量
    uint32x4_t w[64];

    // 初始化消息调度数组
    for (int i = 0; i < 16; i++) {
        w[i] = vreinterpretq_u32_u8(vrev32q_u8(vld1q_u8(&data[i * 4])));
    }
    for (int i = 16; i < 64; i++) {
        w[i] = vaddq_u32(
            vsha256su1q_u32(w[i - 16], w[i - 15]),
            vsha256su0q_u32(w[i - 2], w[i - 7])
        );
    }

    // 主循环
    for (int i = 0; i < 64; i++) {
        uint32x4_t t1 = vaddq_u32(
            vaddq_u32(vsha256hq_u32(abcd, efgh, w[i]), efgh),
            vld1q_u32(&k[i])
        );

        uint32x4_t t2 = vsha256hq_u32(abcd, efgh, w[i]);

        efgh = abcd;
        abcd = vaddq_u32(t1, t2);
    }

    // 更新最终状态
    vst1q_u32(&state[0], vaddq_u32(abcd, vld1q_u32(&state[0])));
    vst1q_u32(&state[4], vaddq_u32(efgh, vld1q_u32(&state[4])));
}

// 主接口函数
void calculate_sha256(const unsigned char* data, int data_len, uint8_t hash[32]) {
    // 初始化哈希状态
    uint32_t state[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    uint8_t block[64] = {0};
    int full_blocks = data_len / 64;
    int remaining = data_len % 64;

    // 处理完整的 512-bit 块
    for (int i = 0; i < full_blocks; i++) {
        sha256_transform_neon(data + i * 64, state);
    }

    // 处理剩余数据并填充
    memcpy(block, data + full_blocks * 64, remaining);
    block[remaining] = 0x80; // 添加结束标志

    if (remaining >= 56) {
        // 如果剩余数据长度超过 56 字节，需要额外的块
        sha256_transform_neon(block, state);
        memset(block, 0, 64);
    }

    // 附加消息长度（以比特为单位）
    uint64_t total_bits = (uint64_t)data_len * 8;
    block[63] = total_bits & 0xFF;
    block[62] = (total_bits >> 8) & 0xFF;
    block[61] = (total_bits >> 16) & 0xFF;
    block[60] = (total_bits >> 24) & 0xFF;
    block[59] = (total_bits >> 32) & 0xFF;
    block[58] = (total_bits >> 40) & 0xFF;
    block[57] = (total_bits >> 48) & 0xFF;
    block[56] = (total_bits >> 56) & 0xFF;

    // 处理最后一个块
    sha256_transform_neon(block, state);

    // 输出哈希值
    for (int i = 0; i < 8; i++) {
        hash[i * 4] = (state[i] >> 24) & 0xFF;
        hash[i * 4 + 1] = (state[i] >> 16) & 0xFF;
        hash[i * 4 + 2] = (state[i] >> 8) & 0xFF;
        hash[i * 4 + 3] = state[i] & 0xFF;
    }
}
