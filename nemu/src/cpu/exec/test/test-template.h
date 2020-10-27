#include "cpu/exec/template-start.h"

#define instr test

static void do_execute() {
    // src1 & src2 -> dest
    OPERAND_W(op_dest, op_dest->val & op_dest->val);
    // 0 -> CF
}

make_instr_helper(r2rm);



#include "cpu/exec/template-end.h"
