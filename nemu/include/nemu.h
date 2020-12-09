#ifndef __NEMU_H__
#define __NEMU_H__

#include "common.h"
#include "memory/memory.h"
#include "cpu/reg.h"
#include "cache/cache.h"
// 每个文件单独编译都没问题，但是链接的时候会出现一大堆的重复定义。
// 所以说, .h文件里面只要有声明就好了, 别随便写上定义, 否则include的时候容易出问题

// nemu.h -> devices.h

#endif
