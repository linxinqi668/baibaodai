#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
    // write src into (esp)
    MEM_W(R_ESP, op_src->val);

    // change the esp
    reg_l(R_ESP) = reg_l(R_ESP) - DATA_BYTE;
}

make_instr_helper(r);



#include "cpu/exec/template-end.h"