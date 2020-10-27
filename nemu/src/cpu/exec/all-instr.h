#include "prefix/prefix.h"

#include "data-mov/mov.h"
#include "data-mov/xchg.h"

// my instruction
#include "call/call.h" // rel, 32bit.   no ELAGS.

#include "push/push.h" // reg, 32bit.   no ELAGS.

#include "test/test.h" // r2rm, 8 or 16 or 32 bit.
                       // 有-v

#include "jcc/je.h" // rel, 8bit.

#include "cmp/cmp.h" // 8 bit imm to 16 or 32 bit rm.
                     // 有-v

#include "pop/pop.h" // reg, 16 or 32 bit.
                     // 有-v



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
