/*
 * @Author: your name
 * @Date: 2020-10-28 12:50:44
 * @LastEditTime: 2020-11-06 15:52:46
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: /NEMU2020/nemu/src/cpu/exec/sub/sub.c
 */

#define DATA_BYTE 4
#include "sub-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "sub-template.h"
#undef DATA_BYTE


make_helper_v(sub_ib2rm);
make_helper_v(sub_rm2r);
make_helper_v(sub_r2rm);
make_helper_v(sub_i2rm);
make_helper_v(sub_i2A);