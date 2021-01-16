#define BLOCK_BIT 6
#define LOG_CACHE_SIZE 22
#define LOG_LINE_PER_SET 4
#define LINE_PER_SET 1 << LOG_LINE_PER_SET
#define CACHE_SIZE 1 << LOG_CACHE_SIZE
#define SET_INDEX_BIT (LOG_CACHE_SIZE - LOG_LINE_PER_SET - BLOCK_BIT)
#define TAG_BIT (32 - SET_INDEX_BIT - BLOCK_BIT)
#define BLOCK_SIZE (1 << BLOCK_BIT)
#define SET_NUM (1 << SET_INDEX_BIT)
#define cache_layer L2

/*  Size of block = 2 ^ BLOCK_BIT
 *  Number of lines in each set = 2 ^ LOG_LINE_PER_SET
 *  Size of cache = 2 ^ LOG_CACHE_SIZE
 *  Number of set bits = LOG_CACHE_SIZE - LOG_LINE_PER_SET- BLOCK_BIT
 *  Number of tag bits = 32(nemu) - Number of set bits - BLOCK_BIT
 */

#include "namespace-start.h"