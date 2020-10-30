#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
    // change the esp
    reg_l(R_ESP) = reg_l(R_ESP) - DATA_BYTE;

    // write src into (esp)
    MEM_W(reg_l(R_ESP), op_src->val);

    if (cpu.eip == 0x1000c0) {
        printf("val is : %x\n", op_src->val);
    }

    print_asm_template1();
}


make_instr_helper(r);
make_instr_helper(rm);
make_instr_helper(i);



#include "cpu/exec/template-end.h"