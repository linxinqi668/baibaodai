
#include "./cache/helper-functions.h"

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