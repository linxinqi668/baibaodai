#include "cpu/exec/template-start.h"

make_helper(concat(stos_, SUFFIX)) {
    // printf("safe start.\n");
    current_sreg = R_ES;
    swaddr_t addr_dest = reg_l(R_EDI);
    printf("%d\n", current_sreg);

    MEM_W(addr_dest, REG(R_EAX));

    if (cpu.EFLAGS.DF == 0)
        reg_l(R_EDI) += DATA_BYTE;
    else
        reg_l(R_EDI) -= DATA_BYTE;
    // printf("safe end.\n\n");
    print_asm_template1();
    return 1;
}


#include "cpu/exec/template-end.h"
