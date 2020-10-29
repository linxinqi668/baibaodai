#include "cpu/exec/template-start.h"


make_helper(concat(rep_movs_, SUFFIX) ) {
    // move (ecx) dwords from [esi] to [edi]
    int count = REG(R_ECX);
    swaddr_t addr_src = REG(R_ESI);
    swaddr_t addr_dest = REG(R_EDI);
    while (count--) {
        MEM_W(
            addr_dest, MEM_R(addr_src)
        );
        addr_dest += DATA_BYTE;
        addr_src += DATA_BYTE;
    }

    return 2;
}


#include "cpu/exec/template-end.h"
