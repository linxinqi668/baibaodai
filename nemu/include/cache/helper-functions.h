#ifndef HELPER_H
#define HELPER_H

#include "common.h"

/* 辅助函数 */
uint32_t unalign_rw_helper(unalign* addr, size_t len);
uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

#endif