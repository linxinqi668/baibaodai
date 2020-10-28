#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
    // change the esp
    reg_l(R_ESP) = reg_l(R_ESP) - DATA_BYTE;

    // write src into (esp)
    MEM_W(reg_l(R_ESP), op_src->val);
}


make_instr_helper(r);
make_instr_helper(rm);
make_instr_helper(i);



#include "cpu/exec/template-end.h"