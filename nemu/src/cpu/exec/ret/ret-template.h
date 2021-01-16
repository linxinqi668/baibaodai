#include "cpu/exec/template-start.h"

#define instr ret

#if DATA_BYTE == 4
make_helper(ret) {
    current_sreg = R_SS;
    // set eip
    cpu.eip = MEM_R( REG(R_ESP) );

    // change esp
    REG(R_ESP) = REG(R_ESP) + DATA_BYTE;

    print_asm_template1();

    return 0;
}
#endif

static void do_execute() {
    current_sreg = R_SS;
    // pop dword to eip
    cpu.eip = swaddr_read(reg_l(R_ESP), 4);

    // change esp ？？？？？？？
    reg_l(R_ESP) += (int16_t)op_src->val;

    print_asm_template1();
}

make_instr_helper(i);


#include "cpu/exec/template-end.h"
