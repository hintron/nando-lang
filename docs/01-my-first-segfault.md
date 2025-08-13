# Task

Fix a C program that is dereferencing (`*`) a null pointer.


## Background

A null pointer is a pointer that has a value of 0. When you dereference it, what
happens is that your program tries to read the value of address 0. Address 0 is
not in an accessible range of your program's virtual memory address space
(assigned by the OS). So the OS kills your program with a segmentation fault.

This is a very common error.

## Extra Credit

Change the value of the pointer from 0 to something 0xFFFF and see if you still
get a segmentation fault.
