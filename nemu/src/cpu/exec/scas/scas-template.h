#include "cpu/exec/template-start.h"

make_helper(concat(scas_, SUFFIX)) {
    // printf("safe start.\n");
    swaddr_t addr_dest = reg_l(R_EDI);

    DATA_TYPE_S dest = MEM_R(addr_dest);
    DATA_TYPE_S src = REG(R_EAX);
    // 就是src - dest. 没错.
    DATA_TYPE_S minus_res = src - dest;

    if (cpu.EFLAGS.DF == 0)
        reg_l(R_EDI) += DATA_BYTE;
    else
        reg_l(R_EDI) -= DATA_BYTE;
    
    // set ZF
    cpu.EFLAGS.ZF = (minus_res == 0) ? 1 : 0;

    // set PF
    uint8_t low_byte = minus_res;
    uint32_t count;
    for (count = 0; low_byte; ++count)
        low_byte &= (low_byte - 1); // 不断清除右边的1
    cpu.EFLAGS.PF = (count % 2 == 0) ? 1 : 0;


    // set SF
    cpu.EFLAGS.SF = (DATA_TYPE)minus_res >> (DATA_BYTE * 8 - 1);

    // set OF in subtraction.
    // 符号不同, 且结果与减数符号相同.
    if (
        (!(((DATA_TYPE)minus_res >> (DATA_BYTE * 8 - 1)) ^
        ((DATA_TYPE)dest >> (DATA_BYTE * 8 - 1)))) &&
        (((DATA_TYPE)dest >> (DATA_BYTE * 8 - 1)) ^
        ((DATA_TYPE)src >> (DATA_BYTE * 8 - 1)))
    )
        cpu.EFLAGS.OF = 1;
    else
        cpu.EFLAGS.OF = 0;

    // set CF in subtraction.
    if ((DATA_TYPE)src < (DATA_TYPE)dest)
        cpu.EFLAGS.CF = 1;
    else
        cpu.EFLAGS.CF = 0;
    // printf("safe end.\n\n");

    print_asm_template1();
    return 1;
}


#include "cpu/exec/template-end.h"
