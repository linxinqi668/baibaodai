#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
    // write into dest.
    DATA_TYPE val = MEM_R( reg_l(R_ESP) );
    OPERAND_W(op_dest, val);

    // change the esp.
    reg_l(R_ESP) = reg_l(R_ESP) + DATA_BYTE;
}

make_instr_helper(r);


#include "cpu/exec/template-end.h"
