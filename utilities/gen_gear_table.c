#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

/**
 * 生成gear_table并打印
 * 
 * 此函数将生成256个64位随机值，用于FastCDC的gear-based rolling hash。
 * 在生产环境中，您可能希望使用固定随机种子或从/dev/urandom中读取随机值以保证
 * 分布性和重现性。这里为简化展示直接使用srand(time(NULL))和rand()。
 */

int main(void) {
    uint64_t gear_table[256];
    srand((unsigned)time(NULL));

    for (int i = 0; i < 256; i++) {
        uint64_t val = 0;
        for (int b = 0; b < 8; b++) {
            val = (val << 8) | (uint64_t)(rand() & 0xFF);
        }
        gear_table[i] = val;
    }

    // 打印gear_table，以C数组形式输出
    printf("static const uint64_t gear_table[256] = {\n");
    for (int i = 0; i < 256; i++) {
        printf("    0x%016llXULL%s\n", (unsigned long long)gear_table[i], i == 255 ? "" : ",");
    }
    printf("};\n");

    return 0;
}
