#include "cpu/exec/template-start.h"

make_helper(ret) {
    // set eip
    cpu.eip = MEM_R( REG(R_ESP) );

    // change esp
    REG(R_ESP) = REG(R_ESP) + DATA_BYTE;

    return 0;
}


#include "cpu/exec/template-end.h"
