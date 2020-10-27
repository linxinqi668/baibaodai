#include "cpu/exec/template-start.h"

make_helper( concat(je_rel_, SUFFIX) ) {
    // decode
    DATA_TYPE_S rel = instr_fetch(eip + 1, DATA_BYTE);

    // judge ZF
    if (cpu.EFLAGS.ZF == 1)
        cpu.eip = cpu.eip + 1 + DATA_BYTE + rel;
    else
        cpu.eip = cpu.eip + 1 + DATA_BYTE;

    return 0;
}


#include "cpu/exec/template-end.h"
