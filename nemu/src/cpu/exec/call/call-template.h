#include "cpu/exec/template-start.h"

#define instr call

make_helper( concat(call_rel_, SUFFIX) ) {
    // push address of next instruction.
    REG(R_ESP) = REG(R_ESP) - DATA_BYTE;
    MEM_W( REG(R_ESP), eip + 0x5);

    // decode
    DATA_TYPE_S rel = instr_fetch(eip + 1, DATA_BYTE);

    // set eip only for 4 byte(i am lazy~)
    cpu.eip = cpu.eip + 1 + DATA_BYTE + rel;

    // ret
    print_asm_template1();
    return 0;
}

static void do_execute() {
    // call 32bit r/m
    cpu.eip = op_src->val - 1; // "2" for this instr.
}


make_instr_helper(rm);







#include "cpu/exec/template-end.h"