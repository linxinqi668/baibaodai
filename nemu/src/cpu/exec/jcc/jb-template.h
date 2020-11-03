#include "cpu/exec/template-start.h"


make_helper( concat(jb_rel_, SUFFIX) ) {
    // decode
    DATA_TYPE_S rel = instr_fetch(eip + 1, DATA_BYTE);

    // judge
    if (cpu.EFLAGS.CF == 1)
        cpu.eip = cpu.eip + 1 + DATA_BYTE + (int32_t)rel;
    else
        cpu.eip = cpu.eip + 1 + DATA_BYTE;

    print_asm_template1();
    return 0;
}




#include "cpu/exec/template-end.h"