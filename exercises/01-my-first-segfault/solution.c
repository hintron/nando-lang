////////////////////////////////////////////////////////////////////////////////
// My First Segfault
////////////////////////////////////////////////////////////////////////////////
// Task:
// * Something is wrong with y_ptr. Fix it so that it points to y instead of NULL.
// 
// See https://github.com/hintron/nando-lang/blob/main/exercises/01-my-first-segfault/background.md
// for more information on segfaults.
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>

int main() {
    int x = 1337; // Value of x
    int *x_ptr = &x; // Pointer to x
    printf("x addr: %p\n", x_ptr);
    printf("x val: %d\n", *x_ptr);
    fflush(stdout); // Force printf to write before segfault occurs

    int y = *x_ptr + 2000; // Value of y (what do you think it will be?)
    int *y_ptr = 0; // Pointer to y (initialized to NULL or 0)

    y_ptr = &y; // Fix y_ptr to point to y

    printf("y addr: %p\n", y_ptr);
    printf("y val: %d\n", *y_ptr);

    return 0;
}
