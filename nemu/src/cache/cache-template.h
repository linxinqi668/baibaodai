


#include "cache/cache-template.h"

// do some computation.
#define BLOCK_SIZE (1 << BLOCK_BIT)
#define SET_NUM (1 << SET_INDEX_BIT)

#define M_DEBUG

// implement the member funtions.

/* 判断缓存是否命中 */
int find(Cache* cache, uint32_t addr) {
    // 解析地址
    printf("%d\n", SET_INDEX_BIT);
    uint32_t tag = addr >> (SET_INDEX_BIT + BLOCK_BIT);
    uint32_t set_ind = addr << (TAG_BIT) >> (TAG_BIT + BLOCK_BIT);

#ifdef M_DEBUG
    printf("this is find function............\n");
    printf("tag is: %x\n", tag);
    printf("set ind is: %x\n", set_ind);
#endif

    // 查找
    int i;
    for (i = 0; i < LINE_PER_SET; i++)
        if (cache->m_set[set_ind][i].m_tag == tag
            && cache->m_set[set_ind][i].is_valid) {
            printf("cache tag is : %x\n", cache->m_set[set_ind][i].m_tag);
            printf("%d\n", cache->m_set[set_ind][i].is_valid);
            return i;
        }
    return -1; // 返回-1表示没找到
}

/* 底层接口 处理一定对齐的数据 */
unalign* align_read(Cache* cache, uint32_t addr) {
    // 解析地址
    uint32_t tag = addr >> (SET_INDEX_BIT + BLOCK_BIT);
    uint32_t set_ind = addr << (TAG_BIT) >> (TAG_BIT + BLOCK_BIT);
    uint32_t block_ind = addr << (32 - BLOCK_BIT) >> (32 - BLOCK_BIT);

    // assert(tag != 0x3fff);

    // 判断是否存在该块
    int line_ind = find(cache, addr);
    bool is_exist = (line_ind == -1) ? false : true;

#ifdef M_DEBUG
    printf("lind ind: %d\n", line_ind);
    printf("is_exist: %d\n", is_exist);
    printf("block ind: %u\n", block_ind);
#endif
    
    // 找不到的话就先替换
    if (!is_exist) {
        int i;
        // 随机选取一行进行替换
        // line_ind = rand() % LINE_PER_SET;
        line_ind = 0;
        // printf("line to rp: %d\n", line_ind);
        // printf("line per set: %d\n", LINE_PER_SET);
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

    int i;
    for (i = 0; i < 36; i++)
        printf("%x ", (__u_char)cache->m_set[set_ind][line_ind].m_block[i]);
    printf("\n");

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
    assert(len > 0);
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
            return -1;
            break;
    }
}

/* read len bytes from cache */
uint32_t cache_read(Cache* cache, uint32_t addr, size_t len) {
    // assert(addr < 0x600000);
    printf("start read........................\n");
#ifdef M_DEBUG
        printf("total_len: %d\n", (int)len);
#endif
    // 准备结果
    uint32_t result = 0;

    // 判断读取是否对齐
    uint32_t addr_st = addr >> BLOCK_BIT << BLOCK_BIT; // 块的起始地址
    // printf("%x", (-1) >> (32 - BLOCK_BIT)); 算数右移... 大意了
    uint32_t addr_ed = addr_st + ((uint32_t)(-1) >> (32 - BLOCK_BIT)); // 块的终止地址
    bool is_unalign = (addr_ed < addr + len - 1) ? true : false;
#ifdef M_DEBUG
    printf("addr is: %x\n", addr);
    printf("addr_st: %x, addr_ed: %x, is_unalign: %d\n",
            addr_st, addr_ed, is_unalign);
#endif
    if (is_unalign) {
        size_t len_1 = addr_ed - addr + 1;
        size_t len_2 = len - len_1;
#ifdef M_DEBUG
        printf("len_1: %d len_2: %d\n", (int)len_1, (int)len_2);
#endif
        unalign* p1 = align_read(cache, addr); // low bit.
        unalign* p2 = align_read(cache, addr + len_1);
        result = (unalign_rw_helper(p1, len_1) << (len_2 * 8)) // shift to store p2.
                 + unalign_rw_helper(p2, len_2);
    } else {
        // 对齐了就直接读取
        unalign* data = align_read(cache, addr);
        result = unalign_rw_helper(data, len);
    }
#ifdef M_DEBUG
    // int i;
    // for (i = 0; i < 10; i++)
    //     printf("%x ", (__u_char)dram_read(addr + i, len));
    // printf("\n");
    // uint32_t answer = dram_read(addr, len);
    // printf("the answer is: %x\n", answer);
    // printf("read result is: %x\n", result);
    // assert(answer == result);
#endif
    printf("end read........................\n\n");
    return result;
}

/* write cache */
void cache_write(Cache* cache, uint32_t addr, uint32_t data, size_t len) {
    printf("start write........................\n");
    // 判断是否存在该块
    int line_ind = find(cache, addr);
    bool is_exist = (line_ind == -1) ? false : true;

    char* _data = (char *)&data + 3; // shift the pointer to lower bit.

    // 如果存在就单独处理, 因为采取了写直通, 所以内存必定修改
    if (is_exist) {
        // 判断读取是否对齐
        uint32_t addr_st = addr >> BLOCK_BIT << BLOCK_BIT; // 块的起始地址
        uint32_t addr_ed = addr_st + ((uint32_t)(-1) >> (32 - BLOCK_BIT)); // 块的终止地址
        bool is_unalign = (addr_ed < addr + len - 1) ? true : false;

        if (is_unalign) {
            size_t len_1 = addr_ed - addr + 1;
            size_t len_2 = len - len_1;
        #ifdef M_DEBUG
            printf("len_1: %d len_2: %d\n", (int)len_1, (int)len_2);
        #endif
            // 找到指针
            printf("xxxxxxxxxxxxxxx\n");
            char* p1 = (char *)align_read(cache, addr);
            char* p2 = (char *)align_read(cache, addr + len_1);
            printf("yyyyyyyyyyyyyyyy\n");
            // 写入
            int i;
            for (i = 0; i < len_1; i++, _data--, p1++)
                *p1 = *_data;
            printf("yyyyyyyyyyyyyyyy\n");
            for (i = 0; i < len_2; i++, _data--, p2++)
                *p2 = *_data;
            printf("yyyyyyyyyyyyyyyy\n");
        } else {
            char* p = (char *)align_read(cache, addr);
            int i;
            for (i = 0; i < len; i++, _data++, p++)
                *p = *_data;
        }
    }

    // 修改内存
    dram_write(addr, len, data);
    printf("end write................\n");
}

void init_cache() {
    printf("start initializa the cache...\n");
    int i, j;
    for (i = 0; i < SET_NUM; i++)
        for (j = 0; j < LINE_PER_SET; j++)
            M_CACHE.m_set[i][j].is_valid = false;
    M_CACHE.m_cache_read = cache_read;
    M_CACHE.m_cache_write = cache_write;
    printf("load cache done.\n");
}

#undef BLOCK_SIZE
#undef SET_NUM