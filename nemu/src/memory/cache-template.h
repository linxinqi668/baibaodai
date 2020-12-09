
#include <stdlib.h>
#include <macro.h>
#include <common.h>

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
    uint32_t (*m_cache_read) (struct cache* cache, uint32_t addr, size_t len);

    /* write cache */
    void (*m_cache_write) (struct cache* cache, uint32_t addr, uint32_t data, size_t len);

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

/* 判断缓存是否命中 */
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

/* 底层接口 处理一定对齐的数据 */
unalign* align_read(Cache* cache, uint32_t addr) {
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
        uint32_t byte_addr = addr >> BLOCK_BIT << BLOCK_BIT;
        
        // read a block. this performance can be proved.
        for (i = 0; i < BLOCK_SIZE; i++, byte_addr++) {
            // read a byte
            char data = dram_read(byte_addr, 1);
            // wirte it into the cache block.
            cache->m_set[set_ind][line_ind].m_block[i] = data;
        }
        // set the line
        cache->m_set[set_ind][line_ind].is_valid = true;
        cache->m_set[set_ind][line_ind].m_tag = tag;
    }

    char* data_addr = cache->m_set[set_ind][line_ind].m_block + block_ind;
    return (unalign*)data_addr;
}

/* unalign 是一个结构体
 * 我觉得使用方式就是：
 * 1. 检测是否会越界
 * 2. 如果会越界就分成两部分，两部分都是相同的操作
 * 3. 这边是利用了：结构体指针指向哪里，哪里的数据就会被当做结构体
 */

/* 辅助函数 */
uint32_t unalign_rw_helper(unalign* addr, size_t len) {
    char c = len;
    switch (c) {
        case 1:
            return unalign_rw(addr, 1);
        case 2:
            return unalign_rw(addr, 2);
        case 3:
            return unalign_rw(addr, 3);
        case 4:
            return unalign_rw(addr, 4);
        default:
            printf("you get here.. it's not a good thing.\n");
            return -1;
            break;
    }
}

/* read len bytes from cache */
/* 这可能跨越block */
uint32_t cache_read(Cache* cache, uint32_t addr, size_t len) {
    // 准备结果
    uint32_t result = 0;

    // 判断读取是否对齐
    uint32_t addr_st = addr >> BLOCK_BIT << BLOCK_BIT; // 块的起始地址
    uint32_t addr_ed = addr_st + ((-1) >> (32 - BLOCK_BIT)); // 块的终止地址
    bool is_unalign = (addr_ed < addr + len) ? true : false;

    if (is_unalign) {
        size_t len_1 = addr_ed - addr + 1;
        size_t len_2 = len - len_1;
        unalign* p1 = align_read(cache, addr);
        unalign* p2 = align_read(cache, addr + len_1);
        result = (unalign_rw_helper(p1, len_1) << (len_2 * 8)) // shift to store p2
                 + unalign_rw_helper(p2, len_2);
    } else {
        // 对齐了就直接读取
        unalign* data = align_read(cache, addr);
        result = unalign_rw_helper(data, len);
    }
    
    return result;
}


