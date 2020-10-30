#include <stdio.h>
#include <string.h>
#include "trap.h"

char buf[128];


int main() {

	// sprintf(buf, "%s", "0"); // 一个字符都写不进去.
	// sprintf(buf, "%s", "1"); // 过不去.	
	// nemu_assert(strcmp(buf, "1") == 0);

	// nemu_assert(strlen("xxxx") == 4);

	sprintf(buf, "%s", "Hello world!\n"); // 过不去.	
	nemu_assert(strcmp(buf, "Hello world!\n") == 0);

	sprintf(buf, "%d + %d = %d\n", 1, 1, 2);
	nemu_assert(strcmp(buf, "1 + 1 = 2\n") == 0);

	// sprintf(buf, "%d + %d = %d\n", 2, 10, 12);
	// nemu_assert(strcmp(buf, "2 + 10 = 12\n") == 0);

	return 0;
}
