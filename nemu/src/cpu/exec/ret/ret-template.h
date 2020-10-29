#include "cpu/exec/template-start.h"

#define instr ret

#if DATA_BYTE == 4
make_helper(ret) {
    // set eip
    cpu.eip = MEM_R( REG(R_ESP) );

    // change esp
    REG(R_ESP) = REG(R_ESP) + DATA_BYTE;

    return 0;
}
#endif

static void do_execute() {
    // pop dword to eip
    cpu.eip = swaddr_read(reg_l(R_ESP), 4);

    // change esp ？？？？？？？
    reg_l(R_ESP) += (int16_t)op_src->val;
}

make_instr_helper(i);


#include "cpu/exec/template-end.h"
