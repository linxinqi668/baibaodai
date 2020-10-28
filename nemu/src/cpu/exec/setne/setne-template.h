#include "cpu/exec/template-start.h"

#define instr setne

static void do_execute() {
    // if condition: r/m <- 1.
    // else: r/m <- 0.

    if (cpu.EFLAGS.ZF == 0)
        OPERAND_W(op_src, 1);
    else
        OPERAND_W(op_src, 0);
}

make_instr_helper(rm);



#include "cpu/exec/template-end.h"
