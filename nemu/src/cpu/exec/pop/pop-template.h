#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
    // write into src... FUCK.
    DATA_TYPE val = MEM_R( reg_l(R_ESP) );
    OPERAND_W(op_src, val);

    // debug.
    // if (cpu.eip == 0x100053) {
    //     printf("dest is: %s", regsl[op_src->reg]);
    // }

    // change the esp.
    reg_l(R_ESP) = reg_l(R_ESP) + DATA_BYTE;
}

make_instr_helper(r);


#include "cpu/exec/template-end.h"
