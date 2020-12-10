#ifndef __NEMU_H__
#define __NEMU_H__

#include "common.h"
#include "memory/memory.h"
#include "cpu/reg.h"
#include "cache/L1-cache.h"
// 每个文件单独编译都没问题，但是链接的时候会出现一大堆的重复定义。
// 所以说, .h文件里面只要有声明就好了, 别随便写上定义, 否则include的时候容易出问题


// 编译的全过程
// 1. 每一个.c文件中的头文件都被替换成文字， 并且分别编译。所以你.c文件中使用到的函数
// 都必须先声明一下。
// 2. 然后生成的各个.o文件链接，形成最终的可执行程序.

// nemu.h -> devices.h

#endif
