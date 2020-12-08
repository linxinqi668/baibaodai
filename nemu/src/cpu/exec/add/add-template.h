#include "cpu/exec/template-start.h"

#define instr add

static void do_execute() {
    // dest + src
    DATA_TYPE_S src = 0;
    if (op_src->type == OP_TYPE_IMM && op_src->size == 1) { // i to rm.
        // sign extended_src
        int8_t src_ = op_src->val;
        src = src_;
    } else // r to rm.
        src = op_src->val;
    DATA_TYPE_S add_res = op_dest->val + src;

    // debug....
    // if (cpu.eip == 0x10102e) {
    //     printf("src is : %x\n", src);
    //     printf("dest is : %x\n", op_dest->val);
    //     printf("res is : %x\n", add_res);
    // }

    // write
    OPERAND_W(op_dest, add_res);

    // set ZF
    cpu.EFLAGS.ZF = (add_res == 0) ? 1 : 0;

    // set PF
    uint8_t low_byte = add_res;
    uint32_t count;
    for (count = 0; low_byte; ++count)
        low_byte &= (low_byte - 1); // 不断清除右边的1
    cpu.EFLAGS.PF = (count % 2 == 0) ? 1 : 0;


    // set SF
    cpu.EFLAGS.SF = (DATA_TYPE)add_res >> (DATA_BYTE * 8 - 1);

    // set CF in plus.
    uint64_t real_res = (uint64_t)op_dest->val + (uint64_t)op_src->val;
    if ((real_res >> 32) == 1) // 溢出就会为1.
        cpu.EFLAGS.CF = 1;
    else
        cpu.EFLAGS.CF = 0;
    
    // set OF
    // 有符号数运算溢出相当于无符号数的进位.
    cpu.EFLAGS.OF = cpu.EFLAGS.CF;

    // debug:
    // if (cpu.eip == 0x1000aa) {
    //     printf("dest is: %d\n", op_dest->val);
    //     printf("src is: %d\n", src);
    //     printf("res is: %d\n", add_res);
    //     printf("CF is: %d\n", cpu.EFLAGS.CF);
    //     printf("ZF is: %d\n", cpu.EFLAGS.ZF);
    //     printf("OF is: %d\n", cpu.EFLAGS.OF);
    //     printf("SF is: %d\n\n\n\n\n", cpu.EFLAGS.SF);
    //     // assert(cpu.EFLAGS.SF == 1);
    // }

    print_asm_template2();
}

make_instr_helper(ib2rm);
make_instr_helper(r2rm);
make_instr_helper(rm2r);
make_instr_helper(i2rm);

make_helper(concat(add_i2A_, SUFFIX)) {
    // decode. eip -> imm
    int len = concat(decode_i_, SUFFIX)(eip + 1);

    DATA_TYPE_S src = op_src->val;
    DATA_TYPE_S dest = REG(R_EAX);
    DATA_TYPE_S add_res = dest + src;

    REG(R_EAX) = add_res;

    // set ZF
    cpu.EFLAGS.ZF = (add_res == 0) ? 1 : 0;

    // set PF
    uint8_t low_byte = add_res;
    uint32_t count;
    for (count = 0; low_byte; ++count)
        low_byte &= (low_byte - 1); // 不断清除右边的1
    cpu.EFLAGS.PF = (count % 2 == 0) ? 1 : 0;


    // set SF
    cpu.EFLAGS.SF = (DATA_TYPE)add_res >> (DATA_BYTE * 8 - 1);

    // set CF in plus.
    uint64_t real_res = (uint64_t)op_dest->val + (uint64_t)op_src->val;
    if ((real_res >> 32) == 1) // 溢出就会为1.
        cpu.EFLAGS.CF = 1;
    else
        cpu.EFLAGS.CF = 0;
    
    // set OF
    // 有符号数运算溢出相当于无符号数的进位.
    cpu.EFLAGS.OF = cpu.EFLAGS.CF;

    return 1 + len;
}


#include "cpu/exec/template-end.h"