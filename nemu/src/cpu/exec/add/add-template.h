#include "cpu/exec/template-start.h"

#define instr add

static void do_execute() {
    // dest + src
    DATA_TYPE_S src = 0;
    if (op_src->type == OP_TYPE_IMM) { // i to rm.
        // sign extended_src 
        int8_t src_ = op_src->val;
        src = src_;
    } else if (op_src->type == OP_TYPE_REG) // r to rm.
        src = op_src->val;
    DATA_TYPE_S add_res = op_dest->val + src;

    if (cpu.eip == 0x100040) {
        printf("src is : %x\n", src);
        printf("dest is : %x\n", op_dest->val);
        printf("res is : %x\n", add_res);
    }

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
    // if (cpu.eip == 0x100040) {
    //     printf("real_res is : %x\n", (uint32_t)real_res);
    //     printf("add_res is : %x\n", (uint32_t)add_res);
    // }
    if (real_res > (uint64_t)add_res) {
        cpu.EFLAGS.CF = 1;
        // if (cpu.eip == 0x100040) {
        //     printf("hhhhhhhhhhhhhhhhhhh\n");
        // }
    }
    else {
        cpu.EFLAGS.CF = 0;
        // if (cpu.eip == 0x100040) {
        //     printf("hhhhhhhhhhhhhhhhhhh\n");
        // }
    }

    // CF has probelm...
    // if (cpu.eip == 0x100040) {
    //     printf("new CF is : %x\n\n\n", cpu.EFLAGS.CF);
    // }
    
    // set OF
    // 有符号数运算溢出相当于无符号数的进位.
    cpu.EFLAGS.OF = cpu.EFLAGS.CF;
}

make_instr_helper(ib2rm);
make_instr_helper(r2rm);


#include "cpu/exec/template-end.h"