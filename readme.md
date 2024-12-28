# Nando Lang

Nando Lang is a simple language and interpreter specification for educational
purposed. I devised it on a piece of paper to help a web dev friend learn more
about compiled languages and how programs written in higher-level languages
actually run on a CPU.

The main points of this exercise are to show that:

1) The interpreter itself needs to be compiled to machine code; at the end of
   the day, something must be compiled to machine code, and that something is
   usually written in a compiled language like C, C++, Rust, Go, Odin, Zig, etc.

2) The code given to the interpreter does *not* need to be compiled. This mimics
   interpreted languages like JavaScript, Python, or Ruby. You can change the
   program code in the `.nando` files without needing to recompile!


# Task

Write a C program that will act as a simple interpreter. Make it:
1) Accept an input file.
2) Parse each line in the file.
3) Parse space-separated values in each line.
4) Execute the parsed commands and print the variables at the end.
5) Support the following commands:
    * `SET <var> <var|num>`
    * `ADD <var> <var|num>`
    * `SUB <var> <var|num>`
    * `MUL <var> <var|num>`
    * `DIV <var> <var|num>`
6) Execute `program1.nando` and `program2.nando` properly:
   
    Program 1's non-zero results:

    ```
    r0 = 5.0
    r1 = 3.0
    ```

    Program 2's non-zero results:

    ```
    r2 = 1.0
    r3 = 1.0
    r4 = 12.0
    r5 = 16.0
    r6 = 8.0
    ```

`<var>` can be one of ten variables: `r0` through `r9`.

`<var|num>` can be a variable or a floating-point value.

`SET` assigns a value to `<var>`.

`ADD` adds a value to `<var>`.

`SUB` subtracts a value from `<var>`.

`MUL` and `DIV` follow the same pattern, but for multiplication and division.


# Build

To build with clang, do:

```
clang main.c -o nando-interpreter
```

(You can also use other compilers like GCC, MSVC, etc.)

This builds the interpreter as a binary/executable called `nando-interpreter`.

To run, do:

```
./nando-interpreter program1.nando
./nando-interpreter program2.nando
```

# Unicode to UTF-8

See https://en.wikipedia.org/wiki/UTF-8#Description for how to convert code
point to UTF-8 bytes.

See https://www.cogsci.ed.ac.uk/~richard/utf-8.cgi for a tool to convert
unicode code point to UTF-8.