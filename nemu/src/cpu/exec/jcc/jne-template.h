#include "cpu/exec/template-start.h"

make_helper( concat(jne_rel_, SUFFIX) ) {
    // decode
    DATA_TYPE_S rel = instr_fetch(eip + 1, DATA_BYTE);

    // judge ZF
    if (cpu.EFLAGS.ZF == 0)
        cpu.eip = cpu.eip + 1 + DATA_BYTE + (int32_t)rel;
    else
        cpu.eip = cpu.eip + 1 + DATA_BYTE;

    print_asm_template1();
    return 0;
}


#include "cpu/exec/template-end.h"
