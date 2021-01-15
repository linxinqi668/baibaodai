#include "cpu/exec/helper.h"


make_helper(std){
	cpu.EFLAGS.DF = 1;
	print_asm("std");
	return 1;
}