#include "cpu/exec/template-start.h"

make_helper(concat(cdq_, SUFFIX) ) {
    if (REG(R_EAX) < 0)
        reg_l(R_EDX) = 0xffffffff;
    else
        reg_l(R_EDX) = 0x00000000;
    
    return 1;
} 

#include "cpu/exec/template-end.h"