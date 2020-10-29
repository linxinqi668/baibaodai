
#define DATA_BYTE 4
#include "movs-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "movs-template.h"
#undef DATA_BYTE

#define DATA_BYTE 1
#include "scas--template.h" // 防止重名, 所以俩-
#undef DATA_BYTE

make_helper_v(rep_movs);