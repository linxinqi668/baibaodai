#include "cpu/exec/template-start.h"

#define instr sub

static void do_execute () {
	DATA_TYPE ret = op_dest -> val - op_src -> val;
	OPERAND_W(op_dest, ret);

    // if (cpu.eip == 0x100853)
    // {
    //     printf("dest: %d\n", op_dest -> val);
    //     printf("src: %d\n", op_src -> val);
    //     printf("res: %d\n", ret);
    // }

	/* TODO: Update EFLAGS. */
    cpu.ZF = !ret;
    cpu.SF = ret >> ((DATA_BYTE << 3) - 1);
    cpu.CF = (op_dest -> val < op_src -> val);
    int tmp1 = (op_dest -> val) >> ((DATA_BYTE << 3) - 1);
    int tmp2 = (op_src -> val) >> ((DATA_BYTE << 3) - 1);
    cpu.OF = (tmp1 != tmp2 && tmp2 == cpu.SF);
    ret ^= ret >> 4;
    ret ^= ret >> 2;
    ret ^= ret >> 1;
    ret &= 1;
    cpu.PF = !ret;
	print_asm_template2();
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif

make_instr_helper(i2a);
make_instr_helper(i2rm);
make_instr_helper(r2rm);
make_instr_helper(rm2r);


#include "cpu/exec/template-end.h"
