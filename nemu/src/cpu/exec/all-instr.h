#include "prefix/prefix.h"

#include "data-mov/mov.h"
#include "data-mov/xchg.h"

// my instruction
#include "call/call.h" // rel, 32bit.   no ELAGS.

#include "push/push.h" // reg, 32bit.   no ELAGS.
                       // m, 16 or 32 bit.

#include "test/test.h" // r2rm, 8 or 16 or 32 bit.
                       // 有-v

#include "jcc/je.h" // rel, 8bit.

#include "cmp/cmp.h" // 8 bit imm to 16 or 32 bit rm.
                     // 有-v
                     // r2rm, 8 or 16 bit or 32 bit.
                     // 有-v
                     // i2 reg of a. 8 or 16 or 32 bit.

#include "pop/pop.h" // reg, 16 or 32 bit.
                     // 有-v

#include "ret/ret.h" // 32bit.

#include "sub/sub.h" // 8 bit imm to 16 or 32 bit rm.
                     // 有-v
                     // rm2r, 16 or 32 bit.
                     // 有-v.
#include "sbb/sbb.h" // r2rm, 16 or 32 bit.
                     // -v

#include "add/add.h" // 8 bit imm to 16 or 32 bit rm.
                     // r2rm, 16 bit or 32 bit.
                     // -v
                     // rm2r, 16 bit or 32 bit.
                     // -v
#include "adc/adc.h" // the same as add.h


#include "jmp/jmp.h" // rel, 8 bit, 32 bit.
                     // rm, 32 bit.
#include "jcc/jbe.h" // rel, 8 bit, 32 bit.
#include "leave/leave.h" // 改变esp 或者 sp.
#include "setne/setne.h" // rm. 8 bit.

#include "jcc/jne.h" // rel, 8 bit.
#include "jcc/jle.h" // rel, 8 bit.
                     // rel, 16 or 32 bit.
                     // -v.
#include "jcc/jg.h" // rel, 8 bit.
#include "jcc/jl.h" // rel, 8 bit. 
#include "jcc/jge.h" // rel, 8 bit. 
#include "jcc/ja.h" // rel, 8 bit. 

#include "rep/rep.h"
#include "lods/lods.h" // 8 16 32.
#include "stos/stos.h" // 8 bit. 16 32 也是可以的。



#include "arith/dec.h"
#include "arith/inc.h"
#include "arith/neg.h"
#include "arith/imul.h"
#include "arith/mul.h"
#include "arith/idiv.h"
#include "arith/div.h"

#include "logic/and.h"
#include "logic/or.h"
#include "logic/not.h"
#include "logic/xor.h"
#include "logic/sar.h"
#include "logic/shl.h"
#include "logic/shr.h"
#include "logic/shrd.h"

#include "string/rep.h"

#include "misc/misc.h"

#include "special/special.h"
