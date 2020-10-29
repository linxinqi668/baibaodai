#include "cpu/exec/template-start.h"

#define instr shr

static void do_execute () {
	DATA_TYPE src = op_src->val;
	DATA_TYPE dest = op_dest->val;

	uint8_t count = src & 0x1f;
	dest >>= count; // dest已经变了.
	OPERAND_W(op_dest, dest);

	/* TODO: Update EFLAGS. */
	int temp = count;
	DATA_TYPE rm = op_dest->val;
	// uint8_t high_order_bit = 
	// 		(rm >> (DATA_BYTE * 8 - 1) ) & 0x1;
	while (temp) {
		uint8_t low_order_bit = rm & 0x1;
		cpu.EFLAGS.CF = low_order_bit;
		rm = rm / 2; // unsigned div.
		temp--;
	}

	if (count == 1)
		cpu.EFLAGS.OF = 0;

	print_asm_template2();
}

make_instr_helper(rm_1)
make_instr_helper(rm_cl)
make_instr_helper(rm_imm)

#include "cpu/exec/template-end.h"
