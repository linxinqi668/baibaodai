#include "cpu/exec/template-start.h"

#define instr inc

static void do_execute () {
	DATA_TYPE result = op_src->val + 1;
	OPERAND_W(op_src, result);

	/* TODO: Update EFLAGS. */
	// set ZF
    cpu.EFLAGS.ZF = (result == 0) ? 1 : 0;

    // set PF
    uint8_t low_byte = result;
    uint32_t count;
    for (count = 0; low_byte; ++count)
        low_byte &= (low_byte - 1); // 不断清除右边的1
    cpu.EFLAGS.PF = (count % 2 == 0) ? 1 : 0;


    // set SF
    cpu.EFLAGS.SF = (DATA_TYPE)result >> (DATA_BYTE * 8 - 1);

    // set CF in plus.
    uint64_t real_res = (uint64_t)1 + (uint64_t)op_src->val;
    if ((real_res >> 32) == 1) // 溢出就会为1.
        cpu.EFLAGS.CF = 1;
    else
        cpu.EFLAGS.CF = 0;

	print_asm_template1();
}

make_instr_helper(rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
