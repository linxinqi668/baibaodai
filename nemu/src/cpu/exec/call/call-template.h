#include "cpu/exec/template-start.h"

make_helper( concat(call_rm_, SUFFIX) ) {

    // decode
    int len = concat(decode_rm_, SUFFIX) (eip + 1); // eip is pointting to opcode.
    
    // push address of next instruction.
    REG(R_ESP) = REG(R_ESP) - 4;
    MEM_W( REG(R_ESP), eip + len + 1); // 1 for opcode.

    // set eip.
    cpu.eip = op_src->val;

    // ret
    print_asm_template1();
    return 0;
}

make_helper( concat(call_rel_, SUFFIX) ) {
    // push address of next instruction.
    REG(R_ESP) = REG(R_ESP) - 4;
    MEM_W( REG(R_ESP), eip + 0x5);

    // decode
    DATA_TYPE_S rel = instr_fetch(eip + 1, DATA_BYTE);

    // set eip only for 4 byte(i am lazy~)
    cpu.eip = cpu.eip + 1 + DATA_BYTE + rel;

    // ret
    print_asm_template1();
    return 0;
}




#include "cpu/exec/template-end.h"