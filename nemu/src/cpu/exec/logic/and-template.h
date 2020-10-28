#include "cpu/exec/template-start.h"

#define instr and

static void do_execute () {
	// 这样写没问题. 因为是符号扩展, 高位一定是保留的.
	DATA_TYPE result = op_dest->val & op_src->val;
	OPERAND_W(op_dest, result);

	/* TODO: Update EFLAGS. */
	// set CF and OF
	cpu.EFLAGS.CF = 0;
	cpu.EFLAGS.OF = 0;

	// set SF
    cpu.EFLAGS.SF = (DATA_TYPE)result >> (DATA_BYTE * 8 - 1);
    // set ZF
    cpu.EFLAGS.ZF = (result == 0) ? 1 : 0;
	// set PF. a little complex...
    // 截取最后8位
    uint8_t low_byte = result;
    uint32_t count;
    for (count = 0; low_byte; ++count)
        low_byte &= (low_byte - 1); // 不断清除右边的1
    cpu.EFLAGS.PF = (count % 2 == 0) ? 1 : 0;
	

	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
