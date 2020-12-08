

#define BLOCK_BIT 6 // 6 bit
#define LOG_CACHE_SIZE 16
#define LOG_LINE_PER_SET 3
#define LINE_PER_SET 1 << LOG_LINE_PER_SET // 8路组相联映射
#define CACHE_SIZE 1 << LOG_CACHE_SIZE // 64 kb
#define SET_INDEX_BIT (LOG_CACHE_SIZE - LOG_LINE_PER_SET - BLOCK_BIT)
#define TAG_BIT (32 - SET_INDEX_BIT - BLOCK_BIT)

#include "cache-template.h"

#undef BLOCK_BIT
#undef LOG_CACHE_SIZE
#undef LOG_LINE_PER_SET
#undef LINE_PER_SET
#undef CACHE_SIZE
#undef SET_INDEX_BIT
#undef TAG_BIT