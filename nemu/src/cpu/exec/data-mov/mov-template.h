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
	current_sreg = R_DS;
	MEM_W(addr, REG(R_EAX));

	print_asm("mov" str(SUFFIX) " %%%s,0x%x", REG_NAME(R_EAX), addr);
	return 5;
}

make_helper(concat(mov_moffs2a_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	current_sreg = R_DS;
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

make_helper(movzwl) {
	// decode
	int len = decode_rm2r_w(eip + 1);

	// write with zero-extend.
	// 取最后16位, 然后扩展.
	// printf("reg is: %s\n", regsl[op_dest->reg]);
	DATA_TYPE rm_extend = (uint16_t)op_src->val;
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

make_helper(movswl) {
	// decode
	int len = decode_rm2r_w(eip + 1);

	// write with sign-extend.
	// 取最后16位, 然后扩展.
	// printf("reg is: %s\n", regsl[op_dest->reg]);
	DATA_TYPE_S rm_extend = (int16_t)op_src->val;
	OPERAND_W(op_dest, rm_extend);

	return 1 + len;
}
#endif

make_helper( concat(movs_, SUFFIX) ) {
	// no need to decode ^_^.
	// debug.
	// if (cpu.eip == 0x1013d7) {
	// 	printf("\n");
	// 	printf("src addr is: %x\n", reg_l(R_ESI));
	// 	printf("dest addr is: %x\n", reg_l(R_EDI));
	// 	printf("\n");
	// }
	
	// [esi] -> [edi]
	// current_sreg = R_DS;
	// MEM_W(
	// 	reg_l(R_EDI), MEM_R(reg_l(R_ESI))
	// );
	// current_sreg = R_ES;

	// if (cpu.EFLAGS.DF == 0) {
	// 	reg_l(R_EDI) += DATA_BYTE;
	// 	reg_l(R_ESI) += DATA_BYTE;
	// } else {
	// 	reg_l(R_EDI) -= DATA_BYTE;
	// 	reg_l(R_ESI) -= DATA_BYTE;
	// }
	current_sreg = R_DS;
    uint32_t tmp = MEM_R(reg_l(R_ESI));
    current_sreg = R_ES;
    swaddr_write(reg_l(R_EDI),DATA_BYTE,tmp);
	if (cpu.EFLAGS.DF == 0) {
        reg_l(R_EDI) += DATA_BYTE;
        reg_l(R_ESI) += DATA_BYTE;
    }else {
        reg_l(R_EDI) -= DATA_BYTE;
        reg_l(R_ESI) -= DATA_BYTE;
    }
	print_asm("movs");
    return 1;

	return 1;
}

#if DATA_BYTE == 4
make_helper(mov_cr2r){
	uint8_t modrm= instr_fetch(eip + 1,1);
	uint8_t cr_num = (modrm >> 3) & 7; // reg
	uint8_t reg_num = modrm & 7; // r/m
	switch (cr_num){
		case 0:
			reg_l(reg_num) = cpu.cr0.val;
			print_asm("mov CR0 %s",REG_NAME(reg_num));
			break;
		case 3:
			reg_l(reg_num) = cpu.cr3.val;
			print_asm("mov CR3 %s",REG_NAME(reg_num));
			break;
		default:
			break;
	}
	return 2;
}

make_helper(mov_r2cr){
	uint8_t modrm= instr_fetch(eip + 1,1);
	uint8_t cr_num = (modrm >> 3) & 7; // reg
	uint8_t reg_num = modrm & 7; // r/m
	switch (cr_num){
		case 0:
			cpu.cr0.val = reg_l(reg_num);
			print_asm("mov %s CR0",REG_NAME(reg_num));
			break;
		case 3:
			init_tlb();
			cpu.cr3.val = reg_l(reg_num);
			print_asm("mov %s CR3",REG_NAME(reg_num));
			break;
		default:
			break;
	}
	return 2;
}
#endif

#if DATA_BYTE == 2
make_helper(mov_sreg2rm){
	uint8_t modrm= instr_fetch(eip + 1,1);
	uint8_t sreg_num = ((modrm >> 3) & 7); // reg
	uint8_t reg_num = (modrm & 7); // r/m
	cpu.sreg[sreg_num].selector = reg_w(reg_num);
	
	sreg_load(sreg_num);

	print_asm("mov %s SREG[%u]",REG_NAME(reg_num),sreg_num);
	return 2;
}
#endif

#include "cpu/exec/template-end.h"
