#ifndef __L1_CACHE_H__
#define __L1_CACHE_H__

#include "common.h"
#include <stdlib.h>

// 必须有前置声明, 链接的时候自动查找
uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

/*  Size of block = 2 ^ BLOCK_BIT
 *  Number of lines in each set = 2 ^ LOG_LINE_PER_SET
 *  Size of cache = 2 ^ LOG_CACHE_SIZE
 *  Number of set bits = LOG_CACHE_SIZE - LOG_LINE_PER_SET- BLOCK_BIT
 *  Number of tag bits = 32(nemu) - Number of set bits - BLOCK_BIT
 */
#define BLOCK_BIT 6
#define LOG_CACHE_SIZE 16
#define LOG_LINE_PER_SET 3
#define LINE_PER_SET 1 << LOG_LINE_PER_SET
#define CACHE_SIZE 1 << LOG_CACHE_SIZE
#define SET_INDEX_BIT (LOG_CACHE_SIZE - LOG_LINE_PER_SET - BLOCK_BIT)
#define TAG_BIT (32 - SET_INDEX_BIT - BLOCK_BIT)

#define cache_layer L1

#include "cache-template.h"

#undef BLOCK_BIT
#undef LOG_CACHE_SIZE
#undef LOG_LINE_PER_SET
#undef LINE_PER_SET
#undef CACHE_SIZE
#undef SET_INDEX_BIT
#undef TAG_BIT
#undef cache_layer

#endif