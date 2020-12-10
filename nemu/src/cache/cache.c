

#include "common.h"
#include <stdlib.h>

// 必须有前置声明, 链接的时候自动查找
uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

#define BLOCK_BIT 6 // 6 bit
#define LOG_CACHE_SIZE 16 // cache size = 2 << 16 -> 64kb
#define LOG_LINE_PER_SET 3 // line number = 2 << 3 -> 8
#define LINE_PER_SET 1 << LOG_LINE_PER_SET
#define CACHE_SIZE 1 << LOG_CACHE_SIZE
#define SET_INDEX_BIT (LOG_CACHE_SIZE - LOG_LINE_PER_SET - BLOCK_BIT)
#define TAG_BIT (32 - SET_INDEX_BIT - BLOCK_BIT) // 标记位的长度

#define cache_layer L1

#include "cache-template.h"

#undef BLOCK_BIT
#undef LOG_CACHE_SIZE
#undef LOG_LINE_PER_SET
#undef LINE_PER_SET
#undef CACHE_SIZE
#undef SET_INDEX_BIT
#undef TAG_BIT