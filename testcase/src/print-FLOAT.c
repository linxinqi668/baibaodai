/*
 * @Author: your name
 * @Date: 2020-10-27 01:41:03
 * @LastEditTime: 2020-11-06 17:27:13
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /NEMU2020/testcase/src/print-FLOAT.c
 */
#include <stdio.h>
#include <string.h>
#include "FLOAT.h"

#include "trap.h"

char buf[128];

int main() {
	init_FLOAT_vfprintf();

#ifdef LINUX_RT
	printf("%f\n", FLOAT_ARG(0x00010000));
	printf("%f\n", FLOAT_ARG(0x00013333));
	printf("%f %d\n", FLOAT_ARG(0xfffecccd), 123456);
#else
	sprintf(buf, "%f", FLOAT_ARG(0x00010000));
	nemu_assert(strcmp(buf, "1.000000") == 0);
	
	sprintf(buf, "%f", FLOAT_ARG(0x00013333));
	nemu_assert(strcmp(buf, "1.199996") == 0);

	sprintf(buf, "%f %d", FLOAT_ARG(0xfffecccd), 123456);
	nemu_assert(strcmp(buf, "-1.199996 123456") == 0);
#endif

	return 0;
}
