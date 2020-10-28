#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute() {
    // dest - src
    DATA_TYPE_S src;
    // 如果是立即数, 需要进行符号扩展.
    if(op_src->type == OP_TYPE_IMM) {
        int8_t src_ = op_src->val; // 转为有符号数
        src = src_; // sign extended.
    }
    else
        src = op_src->val;
    DATA_TYPE_S minus_res = op_dest->val - src;

    // debug.
    // if (cpu.eip == 0x10007c) {
    //     printf("dest is: %s, %x\n", regsl[op_dest->reg], op_dest->val);
    //     printf("src is: %s, %x\n", regsl[op_src->reg], src);
    // }

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
    if (
        ((DATA_TYPE)minus_res >> (DATA_BYTE * 8 - 1)) ^
        ((DATA_TYPE)src >> (DATA_BYTE * 8 - 1))
    )
        cpu.EFLAGS.OF = 0;
    else
        cpu.EFLAGS.OF = 1;

    // set CF in subtraction.
    if ((DATA_TYPE)op_dest->val < (DATA_TYPE)src)
        cpu.EFLAGS.CF = 1;
    else
        cpu.EFLAGS.CF = 0;

    // set AF 草, 居然没用到这个标记位.
    
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
    make_instr_helper(ib2rm);
    make_instr_helper(r2rm);
#endif

#if DATA_BYTE == 1
    make_instr_helper(i2r);
#endif



#include "cpu/exec/template-end.h"
