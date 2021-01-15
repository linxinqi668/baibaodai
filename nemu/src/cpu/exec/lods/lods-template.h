#include "cpu/exec/template-start.h"

make_helper(concat(lods_, SUFFIX)) {
    current_sreg = R_DS;
    // load DATA_BYTE to EAX
    // 地址是32位.
    swaddr_t addr_src = reg_l(R_ESI);

    DATA_TYPE val = swaddr_read(addr_src, DATA_BYTE);

    // 目标不一定.
    REG(R_EAX) = val;

    if (cpu.EFLAGS.DF == 0)
        reg_l(R_ESI) += DATA_BYTE;
    else
        reg_l(R_ESI) -= DATA_BYTE;

    print_asm_template1();

    return 1;
}


#include "cpu/exec/template-end.h"
