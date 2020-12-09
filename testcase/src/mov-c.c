#include "trap.h"

int A[2];
int b;

int main() {
	A[0] = 0;
	A[1] = 1;
	// A[2] = 2;
	// A[3] = 3;
	// A[4] = 4;

	b = A[0];
	A[1] = b;
	
	nemu_assert(A[0] == 0);
	nemu_assert(A[1] == 0);
	// nemu_assert(A[2] == 2);
	// nemu_assert(A[3] == 3);
	// nemu_assert(A[4] == 4);
	nemu_assert(b == 0);
	// nemu_assert(A[5] == 3);

	return 0;
}
