#include "common.h"
#include "cpu/decode/decode.h"

/* shared by all helper function */
Operands ops_decoded;

#define DATA_BYTE 1
#include "decode-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "decode-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "decode-template.h"
#undef DATA_BYTE

// myself decode function.
make_helper(decode_ib2rm_l) {
    // src2 is useless.
	int len = decode_rm_l_internal(eip, op_dest, op_src2);
    // decode_imm -> op_src.
	len += decode_i_b(eip + len);
	return len;
}

make_helper(decode_ib2rm_w) {
    // src2 is useless.
	int len = decode_rm_w_internal(eip, op_dest, op_src2);
    // decode_imm -> op_src.
	len += decode_i_b(eip + len);
	return len;
}
