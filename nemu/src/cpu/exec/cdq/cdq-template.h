#include "cpu/exec/template-start.h"

make_helper(concat(cdq_, SUFFIX) ) {
    if (REG(R_EAX) < 0)
        REG(R_EDX) = -1;
    else
        REG(R_EDX) = 0;
    
    return 1;
} 

#include "cpu/exec/template-end.h"