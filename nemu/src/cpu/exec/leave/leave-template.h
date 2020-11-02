#include "cpu/exec/template-start.h"


make_helper( concat(leave_, SUFFIX) ) {
    // ESP <- EBP
    // 这边一定是32位的运算. 因为StackAddressSize是32.
    reg_l(R_ESP) = reg_l(R_EBP);

    // BP or EBP <- pop()
    REG(R_EBP) = MEM_R(reg_l(R_ESP));

    // change ESP
    REG(R_ESP) = REG(R_ESP) + DATA_BYTE;

    print_asm_template1();

    return 1;
}



#include "cpu/exec/template-end.h"
