
#include <stdlib.h>

// do some computation.
#define BLOCK_SIZE (1 << BLOCK_BIT)
#define SET_NUM (1 << SET_INDEX_BIT)

// line struct
typedef struct {
    bool is_valid;
    uint32_t m_tag : TAG_BIT; // used as tag.
    char m_block[ BLOCK_SIZE ]; // used as block.
} line;

// cache struct
typedef struct cache{
    /* members */
    line m_set[ SET_NUM ][ LINE_PER_SET];

    /* functions */

    /* find something in cache */
    bool (*m_find) (struct cache* cache, uint32_t addr);

    /* read cache */
    char (*m_cache_read) (struct cache* cache, uint32_t addr);

    /* write cache */
    void (*m_cache_write) (struct cache* cache, uint32_t addr);

    /* initialize the cache */
    void (*m_init) (struct cache* cache);

    /* nothing else to do. */

} Cache;

// define my cache
Cache CACHE;

// implement the member funtions.
void init_cache(Cache* cache) {
    int i, j;
    for (i = 0; i < SET_NUM; i++)
        for (j = 0; j < LINE_PER_SET; j++)
            cache->m_set[i][j].is_valid = false;
}

int find(Cache* cache, uint32_t addr) {
    // 解析地址
    uint32_t tag = addr >> (SET_INDEX_BIT + BLOCK_BIT);
    uint32_t set_ind = addr << (TAG_BIT) >> (TAG_BIT + BLOCK_BIT);

    // 查找
    int i;
    for (i = 0; i < LINE_PER_SET; i++)
        if (cache->m_set[set_ind][i].m_tag == tag
            && cache->m_set[set_ind][i].is_valid)
            return i;
    return -1; // 返回-1表示没找到
}

char cache_read(Cache* cache, uint32_t addr) {
    // 解析地址
    uint32_t tag = addr >> (SET_INDEX_BIT + BLOCK_BIT);
    uint32_t set_ind = addr << (TAG_BIT) >> (TAG_BIT + BLOCK_BIT);
    uint32_t block_ind = addr << (32 - BLOCK_BIT) >> (32 - BLOCK_BIT);
    
    // 判断是否存在该块
    int line_ind = find(cache, addr);
    bool is_exist = (line_ind == -1) ? false : true;
    
    // 找不到的话就先替换
    if (!is_exist) {
        int i;
        // 随机选取一行进行替换
        line_ind = rand() % LINE_PER_SET;
        uint32_t byte_addr = addr >> BLOCK_BIT << BLOCK_BIT; // 块的起始地址
        // read a block
        for (i = 0; i < BLOCK_SIZE; i++, byte_addr++) {
            // read a byte ??? 为啥不能调用
            char data = dram_read(byte_addr, 1);
            // wirte it into the cache block.
            cache->m_set[set_ind][line_ind].m_block[i] = data;
        }
        // set the line
        cache->m_set[set_ind][line_ind].is_valid = true;
        cache->m_set[set_ind][line_ind].m_tag = tag;
    }

    return cache->m_set[set_ind][line_ind].m_block[block_ind];
}


