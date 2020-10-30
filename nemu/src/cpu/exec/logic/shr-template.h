#include "cpu/exec/template-start.h"

#define instr shr

static void do_execute () {
	DATA_TYPE src = op_src->val;
	DATA_TYPE dest = op_dest->val;

	uint8_t count = src & 0x1f;
	dest >>= count; // dest已经变了.
	OPERAND_W(op_dest, dest);

	/* TODO: Update EFLAGS. */
	// set ZF.
	cpu.EFLAGS.ZF = (dest == 0) ? 1 : 0;

	// set CF.
	int temp = count;
	DATA_TYPE rm = op_dest->val;
	uint8_t sign_bit = 0;
	while (temp) {
		uint8_t low_order_bit = rm & 0x1;
		cpu.EFLAGS.CF = low_order_bit; // CF = 移入的数值.	
		rm = rm / 2; // unsigned div.
		temp--;
		// 保存倒数第二次移位后的符号位.
		if (temp == 1)
			sign_bit = (rm >> (DATA_BYTE * 8 - 1) ) & 0x1;
	}

	// set OF.
	uint8_t sign_bit_last = (rm >> (DATA_BYTE * 8 - 1) ) & 0x1;
	if (count == 1) // 不同的话OF就是1.
		cpu.EFLAGS.OF = sign_bit ^ sign_bit_last;

	// set SF.
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
