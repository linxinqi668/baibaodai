#include "cpu/exec/template-start.h"

make_helper( concat(jmp_rel_, SUFFIX) ) {
    //decode
    DATA_TYPE_S rel = instr_fetch(eip + 1, DATA_BYTE);

    // jmp
    cpu.eip = cpu.eip + 1 + DATA_BYTE + (int32_t)rel;

    // printf("rel is: %d\n", rel);

    print_asm_template1();

    return 0;
}

make_helper( concat(jmp_rm_, SUFFIX) ) {
    // decode
    concat(decode_rm_, SUFFIX) (eip + 1); // eip + 1 point to ModR/M
    
    // jmp
    cpu.eip = op_src->val;
    // printf("jmp to: %x\n", op_src->val);
    // printf("reg is: %s\n", regsl[op_src->reg]);

    return 0;
}




#include "cpu/exec/template-end.h"
