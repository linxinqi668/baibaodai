#include "cpu/exec/template-start.h"

#define instr test

static void do_execute() {
    // src1 & src2 -> dest
    DATA_TYPE_S result = op_dest->val & op_src->val;
    OPERAND_W(op_dest, result);
    // 0 -> CF
    cpu.EFLAGS.CF = 0;
    // 0 -> OF
    cpu.EFLAGS.OF = 0;
    // set SF
    cpu.EFLAGS.SF = (DATA_TYPE)result >> (DATA_BYTE * 8 - 1);
    // set ZF
    cpu.EFLAGS.ZF = (result == 0) ? 1 : 0;
    // set PF. a little complex...
    // 截取最后8位
    uint8_t low_byte = result;
    uint32_t count;
    for (count = 0; low_byte; ++count)
        low_byte &= (low_byte - 1); // 不断清除右边的1
    cpu.EFLAGS.PF = (count % 2 == 0) ? 1 : 0;

    // OK
    print_asm_template2();

}

make_instr_helper(r2rm);
make_instr_helper(i2rm);
make_instr_helper(i2a);



#include "cpu/exec/template-end.h"
