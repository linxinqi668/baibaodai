/*
 * @Author: your name
 * @Date: 2020-10-30 14:22:09
 * @LastEditTime: 2020-11-06 16:57:37
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /NEMU2020/nemu/src/cpu/exec/jcc/js.c
 */

#define DATA_BYTE 1
#include "js-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "js-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "js-template.h"
#undef DATA_BYTE

make_helper_v(js_rel);

