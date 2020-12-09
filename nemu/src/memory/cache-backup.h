
#include <stdlib.h>
#include "common.h"

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
    // 好像不需要
    // bool (*m_find) (struct cache* cache, uint32_t addr);

    /* read cache */
    uint32_t (*m_cache_read) (struct cache* cache, uint32_t addr, size_t len);

    /* write cache */
    void (*m_cache_write) (struct cache* cache, uint32_t addr, uint32_t data, size_t len);

    /* initialize the cache */
    void (*m_init) (struct cache* cache);

    /* nothing else to do. */

} Cache;

// define my cache
extern Cache M_CACHE;

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

    char* data_addr = (char* )cache->m_set[set_ind][line_ind].m_block + block_ind;
    return (unalign *)data_addr;
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
        unalign* p1 = align_read(cache, addr); // low bit.
        unalign* p2 = align_read(cache, addr + len_1);
        result = (unalign_rw_helper(p1, len_1) << (len_2 * 8)) // shift to store p2.
                 + unalign_rw_helper(p2, len_2);
    } else {
        // 对齐了就直接读取
        unalign* data = align_read(cache, addr);
        result = unalign_rw_helper(data, len);
    }
    
    return result;
}

/* write cache */
void cache_write(Cache* cache, uint32_t addr, uint32_t data, size_t len) {
    // 判断是否存在该块
    int line_ind = find(cache, addr);
    bool is_exist = (line_ind == -1) ? false : true;

    char* _data = (char *)&data;
    _data += (4 - len); // shift to real data.

    // 如果存在就单独处理, 因为采取了写直通, 所以内存必定修改
    if (is_exist) {
        // 判断读取是否对齐
        uint32_t addr_st = addr >> BLOCK_BIT << BLOCK_BIT; // 块的起始地址
        uint32_t addr_ed = addr_st + ((-1) >> (32 - BLOCK_BIT)); // 块的终止地址
        bool is_unalign = (addr_ed < addr + len) ? true : false;

        if (is_unalign) {
            size_t len_1 = addr_ed - addr + 1;
            size_t len_2 = len - len_1;
            // 找到指针
            char* p1 = (char *)align_read(cache, addr);
            char* p2 = (char *)align_read(cache, addr + len_1);
            // 写入
            int i;
            for (i = 0; i < len_1; i++, _data++, p1++)
                *p1 = *_data;
            for (i = 0; i < len_2; i++, _data++, p2++)
                *p2 = *_data;
        } else {
            char* p = (char *)align_read(cache, addr);
            int i;
            for (i = 0; i < len; i++, _data++, p++)
                *p = *_data;
        }
    }

    // 修改内存
    dram_write(addr, len, data);
}

#undef BLOCK_SIZE
#undef SET_NUM









#include "common.h"

#ifndef __CACHE_H__
#define __CACHE_H__

#define BLOCK_BIT 6 // 6 bit
#define LOG_CACHE_SIZE 16 // cache size = 2 << 16 -> 64kb
#define LOG_LINE_PER_SET 3 // line number = 2 << 3 -> 8
#define LINE_PER_SET 1 << LOG_LINE_PER_SET
#define CACHE_SIZE 1 << LOG_CACHE_SIZE
#define SET_INDEX_BIT (LOG_CACHE_SIZE - LOG_LINE_PER_SET - BLOCK_BIT)
#define TAG_BIT (32 - SET_INDEX_BIT - BLOCK_BIT) // 标记位的长度

#include "cache_struct_template.h"

#undef BLOCK_BIT
#undef LOG_CACHE_SIZE
#undef LOG_LINE_PER_SET
#undef LINE_PER_SET
#undef CACHE_SIZE
#undef SET_INDEX_BIT
#undef TAG_BIT

#endif