#include "cpu/exec/template-start.h"

make_helper( concat(jmp_rel_, SUFFIX) ) {
    //decode
    DATA_TYPE_S rel = instr_fetch(eip + 1, DATA_BYTE);

    // jmp
    cpu.eip = cpu.eip + 1 + DATA_BYTE + (int32_t)rel;

    return 0;
}

make_helper( concat(jmp_reg_, SUFFIX) ) {
    // jmp
    cpu.eip = op_src->val;

    return 0;
}




#include "cpu/exec/template-end.h"
