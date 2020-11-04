#include <stdio.h>
#include <stdint.h>
#include "../FLOAT.h"

#include <sys/mman.h>

// extern表示变量在别的文件里定义.
// 这边不能是 char....
extern uint_fast32_t _vfprintf_internal;
extern uint_fast32_t _fpmaxtostr;
extern int __stdio_fwrite(char *buf, int len, FILE *stream);

__attribute__((used)) static int format_FLOAT(FILE *stream, FLOAT f) {
	/* TODO: Format a FLOAT argument `f' and write the formating
	 * result to `stream'. Keep the precision of the formating
	 * result with 6 by truncating. For example:
	 *              f          result
	 *         0x00010000    "1.000000"
	 *         0x00013333    "1.199996"
	 */

	// TODO: 人话: 这边要实现一个函数F2f; 然后写到stream里面去.
	// stream这个参数应该不需要动.

	char buf[80];
	int len = sprintf(buf, "0x%08x", f); // 把f的16进制表示写入buf. 0x******
	// 因为f是32位的, 所以最多写8个.

	// TODO: 修改buf.
	return __stdio_fwrite(buf, len, stream);
}

static void modify_vfprintf() {
	/* TODO: Implement this function to hijack the formating of "%f"
	 * argument during the execution of `_vfprintf_internal'. Below
	 * is the code section in _vfprintf_internal() relative to the
	 * hijack.
	 */

	/* 这种注释好漂亮诶.
	 * 从汇编代码中获取的信息.
	 * 1. call指令相对于xxxxinternal的偏移量为:
	 *    0x8048865 - 0x804855f = 0x306 = displacement_call
	 * 	  所以call指令的地址就是:
	 * 	  addr of (_vfprintf_internal) + displacament_call
	 * 
	 * 2. print-FLOAT-linux这段代码会调用init_FLOAT_vfprintf这个函数
	 * 	  从而调用modify_vfprintf来修改内存.
	 *    所以可以用函数指针来计算出运行时的函数地址, 从而计算偏移量.
	 * 
	 * 3. new_rel = old_rel - addr of (_fpmaxtostr) + addr of (format_FLOAT)
	 *    new_eip = eip + new_rel = eip + old_rel - addr1 + addr2
	 * 			  = addr1 - addr1 + addr2
	 * 			  = addr2.
	 * 	  这就完成跳转了.
	 * 
	 * 4. 设置函数参数.. 比较难的样子. 用fact.c来看一下参数的安排方式.
	 *    可以发现参数是从右往压栈的. 需要修改压栈的指令来传参.
	 * 	  先实现跳转吧.
	 * */

	 // 计算call指令的地址. 函数名就是函数的地址.

	 
	 uint_fast32_t addr_vfprintf_internal = (uint_fast32_t)_vfprintf_internal;
	 uint_fast32_t dispacement_call = 0x306;
	 uint_fast32_t addr_call = addr_vfprintf_internal + dispacement_call;

	 printf("call_rel is: %x\n", addr_call);
	 printf("addr of vfprintf_internal is: %x\n", _vfprintf_internal);

	 // 消除保护模式.
	 mprotect(
		 (void *)((addr_call - 100) & 0xfffff000),
		 4096 * 2,
		 PROT_READ | PROT_WRITE | PROT_EXEC
	 );

	 // 修改rel.  1 for opcode.
	 uint_fast32_t * addr_rel = (uint_fast32_t *)(addr_call + 1);
	 printf("addr of _fpmaxtostr is: %x\n", _fpmaxtostr);
	 printf("addr of rel is: %x\n", addr_rel);
	 uint_fast32_t old_rel = *addr_rel;
	 printf("old rel is: %x\n", old_rel);
	 printf("addr of format is: %x\n", format_FLOAT);
	 uint_fast32_t new_rel = old_rel -
	 						 (uint_fast32_t)_fpmaxtostr +
							 (uint_fast32_t)format_FLOAT;
	// 修改内容.
	*addr_rel = new_rel;
	// 应该能完成跳转了.
	

#if 0
	else if (ppfs->conv_num <= CONV_A) {  /* floating point */
		ssize_t nf;
		nf = _fpmaxtostr(stream,
				(__fpmax_t)
				(PRINT_INFO_FLAG_VAL(&(ppfs->info),is_long_double)
				 ? *(long double *) *argptr
				 : (long double) (* (double *) *argptr)),
				&ppfs->info, FP_OUT );
		if (nf < 0) {
			return -1;
		}
		*count += nf;

		return 0;
	} else if (ppfs->conv_num <= CONV_S) {  /* wide char or string */
#endif

	/* You should modify the run-time binary to let the code above
	 * call `format_FLOAT' defined in this source file, instead of
	 * `_fpmaxtostr'. When this function returns, the action of the
	 * code above should do the following:
	 */

#if 0
	else if (ppfs->conv_num <= CONV_A) {  /* floating point */
		ssize_t nf;
		nf = format_FLOAT(stream, *(FLOAT *) *argptr);
		if (nf < 0) {
			return -1;
		}
		*count += nf;

		return 0;
	} else if (ppfs->conv_num <= CONV_S) {  /* wide char or string */
#endif

}

static void modify_ppfs_setargs() {
	/* TODO: Implement this function to modify the action of preparing
	 * "%f" arguments for _vfprintf_internal() in _ppfs_setargs().
	 * Below is the code section in _vfprintf_internal() relative to
	 * the modification.
	 */

#if 0
	enum {                          /* C type: */
		PA_INT,                       /* int */
		PA_CHAR,                      /* int, cast to char */
		PA_WCHAR,                     /* wide char */
		PA_STRING,                    /* const char *, a '\0'-terminated string */
		PA_WSTRING,                   /* const wchar_t *, wide character string */
		PA_POINTER,                   /* void * */
		PA_FLOAT,                     /* float */
		PA_DOUBLE,                    /* double */
		__PA_NOARG,                   /* non-glibc -- signals non-arg width or prec */
		PA_LAST
	};

	/* Flag bits that can be set in a type returned by `parse_printf_format'.  */
	/* WARNING -- These differ in value from what glibc uses. */
#define PA_FLAG_MASK		(0xff00)
#define __PA_FLAG_CHAR		(0x0100) /* non-gnu -- to deal with hh */
#define PA_FLAG_SHORT		(0x0200)
#define PA_FLAG_LONG		(0x0400)
#define PA_FLAG_LONG_LONG	(0x0800)
#define PA_FLAG_LONG_DOUBLE	PA_FLAG_LONG_LONG
#define PA_FLAG_PTR		(0x1000) /* TODO -- make dynamic??? */

	while (i < ppfs->num_data_args) {
		switch(ppfs->argtype[i++]) {
			case (PA_INT|PA_FLAG_LONG_LONG):
				GET_VA_ARG(p,ull,unsigned long long,ppfs->arg);
				break;
			case (PA_INT|PA_FLAG_LONG):
				GET_VA_ARG(p,ul,unsigned long,ppfs->arg);
				break;
			case PA_CHAR:	/* TODO - be careful */
				/* ... users could use above and really want below!! */
			case (PA_INT|__PA_FLAG_CHAR):/* TODO -- translate this!!! */
			case (PA_INT|PA_FLAG_SHORT):
			case PA_INT:
				GET_VA_ARG(p,u,unsigned int,ppfs->arg);
				break;
			case PA_WCHAR:	/* TODO -- assume int? */
				/* we're assuming wchar_t is at least an int */
				GET_VA_ARG(p,wc,wchar_t,ppfs->arg);
				break;
				/* PA_FLOAT */
			case PA_DOUBLE:
				GET_VA_ARG(p,d,double,ppfs->arg);
				break;
			case (PA_DOUBLE|PA_FLAG_LONG_DOUBLE):
				GET_VA_ARG(p,ld,long double,ppfs->arg);
				break;
			default:
				/* TODO -- really need to ensure this can't happen */
				assert(ppfs->argtype[i-1] & PA_FLAG_PTR);
			case PA_POINTER:
			case PA_STRING:
			case PA_WSTRING:
				GET_VA_ARG(p,p,void *,ppfs->arg);
				break;
			case __PA_NOARG:
				continue;
		}
		++p;
	}
#endif

	/* You should modify the run-time binary to let the `PA_DOUBLE'
	 * branch execute the code in the `(PA_INT|PA_FLAG_LONG_LONG)'
	 * branch. Comparing to the original `PA_DOUBLE' branch, the
	 * target branch will also prepare a 64-bit argument, without
	 * introducing floating point instructions. When this function
	 * returns, the action of the code above should do the following:
	 */

#if 0
	while (i < ppfs->num_data_args) {
		switch(ppfs->argtype[i++]) {
			case (PA_INT|PA_FLAG_LONG_LONG):
			here:
				GET_VA_ARG(p,ull,unsigned long long,ppfs->arg);
				break;
			// ......
				/* PA_FLOAT */
			case PA_DOUBLE:
				goto here;
				GET_VA_ARG(p,d,double,ppfs->arg);
				break;
			// ......
		}
		++p;
	}
#endif

}

// 这个函数会被调用.
void init_FLOAT_vfprintf() {
	modify_vfprintf(); //  直接修改内存, 从而修改机器指令达到跳转的目的.
	modify_ppfs_setargs();
}
