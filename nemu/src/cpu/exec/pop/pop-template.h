#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
    current_sreg = R_SS;
    // write into src... FUCK.
    DATA_TYPE val = MEM_R( reg_l(R_ESP) );
    OPERAND_W(op_src, val);

    // debug.
    // if (cpu.eip == 0x10014c) {
    //     printf("dest is: %s", regsl[op_src->reg]);
    // }

    // change the esp.
    reg_l(R_ESP) = reg_l(R_ESP) + DATA_BYTE;

    print_asm_template1();
}

make_instr_helper(r);
make_instr_helper(rm);


#include "cpu/exec/template-end.h"
