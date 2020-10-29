#include "cpu/exec/template-start.h"

make_helper(concat(lods_, SUFFIX)) {
    // load DATA_BYTE to EAX
    swaddr_t addr_src = REG(R_ESI);

    DATA_TYPE val = swaddr_read(addr_src, DATA_BYTE);

    REG(R_EAX) = val;

    return 1;
}


#include "cpu/exec/template-end.h"
