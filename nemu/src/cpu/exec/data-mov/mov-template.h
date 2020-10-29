#include "cpu/exec/template-start.h"

#define instr mov

static void do_execute() {
	OPERAND_W(op_dest, op_src->val);
	print_asm_template2();
}

make_instr_helper(i2r)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

make_helper(concat(mov_a2moffs_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	MEM_W(addr, REG(R_EAX));

	print_asm("mov" str(SUFFIX) " %%%s,0x%x", REG_NAME(R_EAX), addr);
	return 5;
}

make_helper(concat(mov_moffs2a_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	REG(R_EAX) = MEM_R(addr);

	print_asm("mov" str(SUFFIX) " 0x%x,%%%s", addr, REG_NAME(R_EAX));
	return 5;
}

// myself.

#if DATA_BYTE == 4 // must be 32 bit...
make_helper(movzbl) {
	// decode
	int len = decode_rm2r_b(eip + 1);

	// write with zero-extend.
	// 取最后8位, 然后扩展.
	// printf("reg is: %s\n", regsl[op_dest->reg]);
	DATA_TYPE rm_extend = (uint8_t)op_src->val;
	OPERAND_W(op_dest, rm_extend);

	return 1 + len;
}

make_helper(movsbl) {
	// decode
	int len = decode_rm2r_b(eip + 1);

	// write with sign-extend.
	// 取最后8位, 然后扩展.
	// printf("reg is: %s\n", regsl[op_dest->reg]);
	DATA_TYPE_S rm_extend = (int8_t)op_src->val;
	OPERAND_W(op_dest, rm_extend);

	return 1 + len;
}
#endif

make_helper( concat(movs_, SUFFIX) ) {
	// no need to decode ^_^.
	
	// [esi] -> [edi]
	MEM_W(
		reg_l(R_EDI), MEM_R(reg_l(R_ESI))
	);

	return 1;
}

#include "cpu/exec/template-end.h"
