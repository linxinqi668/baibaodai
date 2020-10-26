#include "cpu/exec/template-start.h"

make_helper( concat(call_rel_, SUFFIX) ) {
    // decode
    DATA_TYPE rel = instr_fetch(eip + 1, DATA_BYTE);

    // set eip only for 4 byte(i am lazy~)
    cpu.eip = cpu.eip + 1 + DATA_BYTE + rel;

    // ret
    return 0;
}





#include "cpu/exec/template-end.h"