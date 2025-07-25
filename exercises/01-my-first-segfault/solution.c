////////////////////////////////////////////////////////////////////////////////
// My First Segfault
////////////////////////////////////////////////////////////////////////////////
// Task:
// * Set p to 0, print the address, and then print the value (dereference it).
// * You may need to add `(int *)` to the left of the value to cast it.
// 
// See https://github.com/hintron/nando-lang/blob/main/exercises/01-my-first-segfault/background.md
// for more information on segfaults.
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>

int main() {
    int x = 1337; // Value of x
    int *p = &x; // Pointer to x
    printf("x addr: %p\n", p);
    printf("x val: %d\n", *p);
    fflush(stdout); // Force printf to write before segfault occurs

    // Solution:
    p = 0;
    p = (int *)0xFFFFFFF;
    printf("x addr: %p\n", p);
    printf("x val: %d\n", *p);
    return 1;
}