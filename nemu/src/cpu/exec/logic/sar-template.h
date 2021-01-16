#include "cpu/exec/template-start.h"

#define instr sar

static void do_execute () {
	DATA_TYPE src = op_src->val;
	DATA_TYPE_S dest = op_dest->val;

	uint8_t count = src & 0x1f;
	dest >>= count;
	OPERAND_W(op_dest, dest);

	/* TODO: Update EFLAGS. */
	// set ZF.
	cpu.EFLAGS.ZF = (dest == 0) ? 1 : 0;

	// set CF.
	int temp = count;
	DATA_TYPE_S rm = op_dest->val;
	while (temp) {
		uint8_t low_order_bit = rm & 0x1;
		cpu.EFLAGS.CF = low_order_bit;
		rm = rm / 2;
		temp--;
	}

	// set OF.
	if (count == 1)
		cpu.EFLAGS.OF = 0;

	// set SF.
	uint8_t sign_bit_last = (rm >> (DATA_BYTE * 8 - 1) ) & 0x1;
	cpu.EFLAGS.SF = (sign_bit_last == 1) ? 1 : 0;

	// set PF
    uint8_t low_byte = dest;
    uint32_t cnt;
    for (cnt = 0; low_byte; ++cnt)
        low_byte &= (low_byte - 1); // 不断清除右边的1
    cpu.EFLAGS.PF = (cnt % 2 == 0) ? 1 : 0;

	print_asm_template2();
}

make_instr_helper(rm_1)
make_instr_helper(rm_cl)
make_instr_helper(rm_imm)

#include "cpu/exec/template-end.h"
