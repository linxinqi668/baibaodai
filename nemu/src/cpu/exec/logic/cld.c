#include "cpu/exec/helper.h"


make_helper(cld){
	cpu.EFLAGS.DF = 0;
	print_asm("cld");
	return 1;
}