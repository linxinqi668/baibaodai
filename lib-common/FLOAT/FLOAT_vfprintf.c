#include <stdio.h>
#include <stdint.h>
#include "FLOAT.h"

#include <sys/mman.h>

// 所有的extern变量在反汇编代码中的地址都正常, 但是运行结果都不一样.

// 学到的东西:
// extern只是声明变量而已, 类型无所谓.
// 这边char删了也是可以的.
extern char _vfprintf_internal;
extern char _fpmaxtostr;


extern int __stdio_fwrite(char *buf, int len, FILE *stream);

__attribute__((used)) static int format_FLOAT(FILE *stream, FLOAT f) {
	/* TODO: Format a FLOAT argument `f' and write the formating
	 * result to `stream'. Keep the precision of the formating
	 * result with 6 by truncating. For example:
	 *              f          result
	 *         0x00010000    "1.000000"
	 *         0x00013333    "1.199996"
	 */

	// TODO: 这边要实现一个函数F2f; 然后写到stream里面去.
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

	/* 
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
	 * */

	 // 计算call指令的地址. 函数名就是函数的地址.

	 // 出现的问题: 使用debug查看该程序, 汇编代码是正常的, 但是运行结果不对.
     // 解决办法: extern后面的是变量, 想要获得它的地址必须加上取址运算符.
	 uint_fast32_t addr_vfprintf_internal = (uint_fast32_t)&_vfprintf_internal;
	 uint_fast32_t dispacement_call = 0x306;
	 uint_fast32_t addr_call = addr_vfprintf_internal + dispacement_call;

	//  printf("addr of call is: %x\n", addr_call);
	//  printf("addr of vfprintf_internal is: %x\n", (uint_fast32_t)&_vfprintf_internal);
	 // 消除保护模式.
	 mprotect(
		 (void *)((addr_call - 100) & 0xfffff000),
		 4096 * 2,
		 PROT_READ | PROT_WRITE | PROT_EXEC
	 );

	 // 修改rel.  1 for opcode.
	 uint_fast32_t * addr_rel = (uint_fast32_t *)(addr_call + 1);

	//  debug.
	//  printf("addr of _fpmaxtostr is: %x\n", _fpmaxtostr);
	//  printf("addr of rel is: %x\n", (uint_fast32_t )addr_rel);
	//  printf("addr of format is: %x\n", &format_FLOAT); // format的地址没问题.


	uint_fast32_t old_rel = *addr_rel;
	// old_rel是对的.
	// printf("%x\n", old_rel);
	uint_fast32_t new_rel = old_rel -
							(uint_fast32_t)&_fpmaxtostr +
							(uint_fast32_t)&format_FLOAT;
	// 修改内容.
	*addr_rel = new_rel;

	// gdb可以看到成功进入了format_FLOAT.

	// 修改参数
	/*
	 if函数块.
 	8048e10:	f6 84 24 a5 00 00 00 	testb  $0x8,0xa5(%esp) // 跟8进行比较.
	8048e17:	08 
	// 这里就可以看出argptr所在的寄存器是edx.
	8048e18:	74 04                	je     8048e1e <_vfprintf_internal+0x2e8>
	// fldt的作用是加载内存中的浮点数.
	8048e1a:	db 2a                	fldt   (%edx)
	8048e1c:	eb 02                	jmp    8048e20 <_vfprintf_internal+0x2ea>
	8048e1e:	dd 02                	fldl   (%edx)
	8048e20:	53                   	push   %ebx
	8048e21:	53                   	push   %ebx
	8048e22:	68 cf 8a 04 08       	push   $0x8048acf
	8048e27:	8d 84 24 a4 00 00 00 	lea    0xa4(%esp),%eax
	8048e2e:	50                   	push   %eax
	// 这条指令需要修改栈指针改变的大小.
	// FLAOT类型只需要4个字节, 所以sub 0x4, esp就行 0c -> 04
	8048e2f:	83 ec 0c             	sub    $0xc,%esp -> sub $0x4, %esp.(1)
	// 这一条指令需要改成 push val.
	// 这条指令的功能类似于pop, 也就是说数据已经被pop到了*esp.
	// val是由argptr指向的.
	8048e32:	db 3c 24             	fstpt  (%esp)      // 浮点指令 第二个参数.
	-> push (edx) (2)
	*/

	// (1) 修改sub指令.
	uint8_t * addr_sub_val = (uint8_t *)(addr_call - 11);
	*addr_sub_val = 0x4;

	printf("reached this line1.\n");
	printf("修改后的sub指令为: %x\n", *((uint_fast32_t *)(addr_sub_val-2)));

	// (2 修改为push)
	uint8_t * addr_push_instr = (uint8_t *)(addr_call - 10); // fldt的地址.
	*(addr_push_instr) = 0xff; // 修改为push.
	printf("reached this line2.\n");
	*(addr_push_instr + 1) = 0x32; // (edx);
	printf("reached this line3.\n");
	*(addr_push_instr + 2) = 0x90; // nop;
	printf("reached this line4.\n");

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
