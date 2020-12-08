#include <stdio.h>

typedef struct {
    int a : 10;
    int b : 20;
} test;

int main() {
    test t;
    printf("%d\n", t.a);
    return 0;
}
