
#define DATA_BYTE 4
#include "cmp-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "cmp-template.h"
#undef DATA_BYTE

make_helper_v(cmp_ib2rm);
make_helper_v(cmp_r2rm);