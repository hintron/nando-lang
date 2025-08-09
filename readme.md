# Nando Lang

[![CI](https://github.com/hintron/nando-lang/actions/workflows/ci.yml/badge.svg)](https://github.com/hintron/nando-lang/actions/workflows/ci.yml)

This repo contains a set of beginning exercises and tasks to help you learn the
C programming language and basic CPU architecture.

These tasks are meant for beginners who know very basic programming in a
high-level language. Basic knowledge of functions, variables, strings, loops,
etc. will be assumed by the exercises, though C-specific syntax will not be.

As a prerequisite, please do the "Learn the Basics" portion
of the [Learn C](https://www.learn-c.org/) exercises (at least complete up to
the "Functions" lesson).


# Quickstart

Go to https://github.com/hintron/nando-lang/releases and download the source
code. Extract it to a directory (don't use Git to clone it).

In a terminal, go to that directory and do
```
./run.sh
```

Then, follow the instructions! The instructions will tell you to edit the first
exercise, then rerun `run.sh` to build and run the tests to see if it passes.

(After running `run.sh`, you may do `git init` if you use Git to save your work.)

# C Reference Material


For a basic C syntax reference, see

* [Wikibooks - C Programming](https://en.wikibooks.org/wiki/C_Programming)
* [CS50x 2024 - Lecture 1 - C (YouTube)](https://youtu.be/cwtpLIWylAw?si=6ipzrHEzzsCXtRve)
* [Learn C](https://www.learn-c.org/)
* [C Beginner's Handbook](https://www.freecodecamp.org/news/the-c-beginners-handbook/)

En espanol:

* [Lenguaje C](https://informatica.uv.es/estguia/ATD/apuntes/laboratorio/Lenguaje-C.pdf)
* [Wikilibros - Programación en C](https://es.wikibooks.org/wiki/Programaci%C3%B3n_en_C)
* [Learn C](https://www.learn-c.org/es/)
* [Aprende C en 45 minutos (curso desde cero)](https://www.youtube.com/watch?v=1-qjMS6C960)
* [El Manual para principiantes C](https://www.freecodecamp.org/espanol/news/el-libro-para-principiantes-c-aprende-las-bases-del-lenguaje-de-programacion-c-en-solo-unas-horas/)

# Exercises:

### 00: Hello, world!

Take a program that fails to compile and make it print "Hello, world!"


### 01: My first segfault

Write a program that dereferences a null pointer. Also, write a program that
goes off the end of an array and prints stuff until it causes a segfault.
Teaches pointers.

### 02: My first stack overflow

Write a program that will infinitely recurse a
function. Print out how many times it's called, and print the address of
locals. Calculate the rough size of the stack before it overflows. Then,
try to allocate a large array on the stack and see how big it can get. Teaches
recursion, stack limits, memory.

### 03: My first OOM

Write a program that continually allocates memory until you run
out of memory. Ask the programmer to view the computer memory in real time to
see what happens. Teaches memory, CPUs, memory management.

### 04: My first divide by 0

Write a program that divides by 0.


### My first integer overflow

Write a program that overflows and underflows an
integer. Convert between signed and unsigned, and evaluate integer math and
2's complement. Teaches 2's compliment, integer arithmetic, casting.


# Other

* [Nando Lang](exercises/nando-lang/readme.md) Build a very, very simple language
  interpreter (The original task that this repo is named after).


# Ideas

Create some exercises that will help teach C in a fun way. Support it for MacOS,
Linux (Ubuntu), and WSL (Ubuntu). Have each exercise have a readme page with
explanations, hints, and external resources. Each exercise will also have
bash-based tests for each checkpoint. Only audit certain bits here and there, so
as to not give away the answer.

Exercises to create:

### Numbers do *what*?:
Write a program that takes two small signed numbers, cast
them to unsigned, add them together, print the results, then cast them back to
signed. Also, print the hex at each stage. Show that unsigned and signed
integer arithmetic is the same, because of two's complement. Under the hood,
the CPU has integer add/sub instructions that don't care about the sign.
Also, do multiplication, but notice that the difference is in sign extension
of the upper bits
Resources:
* https://stackoverflow.com/questions/42587607/why-is-imul-used-for-multiplying-unsigned-numbers
* https://www.computerenhance.com/p/q-and-a-22-2023-08-15?utm_campaign=post&utm_medium=web&timestamp=394

### Gulliver's Travels

Write a program with large 64-bit numbers, and print out
each byte via masks. Show that the ints are little endian in memory. Could
also convert to an array of uint8s to show memory contents. Show that
single-byte values are the same order either way. Have some array of u8s,
create a u16 with the MSB 1 and LSB 0. Guess the value (256).
Teaches endianness, byte manipulation, integers, integer ranges, MSB.

### My first floating point inaccuracies

Write a program that accumulates
floating point inaccuracies. Also, do the same thing in Python and Javascript,
and point out that everything uses IEEE floating point under the hood. Maybe
even inspect some assembly? Teaches floating point.

### String shenanigans

Create chars on the stack, and create an array on the
stack above the chars, and then access the array out of bounds to change the
chars and to print a string. Teaches arrays, strings, memory, pointers, data
types.

### Histogram
Process a text file and print a histogram of how many times each
letter is used. Print to screen.

### Treasure hunt
Follow a set of instructions to bit manipulate some garbled
data and extract a secret message. This will teach bitwise operations,
masking, data types, casting, etc.

### Mini REPL
Make a mini repl/shell with some simple commands.

### Image-in that!
Extract an image from garbled data and piece it together,
like a jigsaw puzzle. Teaches bit manipulation, arrays, file IO.

### My first triangle
Run a simple Raylib window. Then, using arrays and bit
manipulation, create a colored RGB triangle. Start with colors being discrete,
then interpolated. Take an image we already worked on and insert part of that
into the triangle somehow. Teaches bit manipulation, arrays, file IO, graphics.

### Out of this world
Create a mini orbiting solar system using the gravitational
equation and planet sprites. Spawn planets with a simple click in a given
direction. Teaches math, simulation, mouse input handling, event loops.

### De Morgan's Lawyer
Reduce some boolean logic in C code to it's most optimal
form using De Morgan's law and other boolean identities.

### Grand C (race between C vs. Python vs. Javascript)
Do a somewhat complicated
text processing problem in vanilla C, Python, and Javascript, and compare the
times. How much faster is C? Also, add in print statements throughout, then
disable them with #ifdefs. How much do print statements slow things down?

### Floating in the C
Write a program that will take a floating point number, put
it in a union, and then mask out the bits to determine what the mantissa,
exponent, etc. are. Do it for doubles as well. Multiply the floating point
number by a value using only bit operations.

### Fix up "Nando lang" to be more high-level (`a = b` instead of `set a b`), and
implement the low level as "Nando asm".

### Ride the Wav
Do some bit manipulations on a piece of simple audio. Maybe even
do a convolution of one piece of audio with another piece to apply an 'echo'
or 'cathedral' effect?

### Cipher Security
Write a program that will use simple ciphers to "encrypt"
data, and to decrypt it again. Teaches encryption.

### Compress Me
Write a program that will do something like Huffman encoding in
order to compress a thing of text. See what kind of savings you can get. Be
able to compress and decompress the same thing. Teaches compression.

### CRC is my BFF
Write a simple program that converts some text into CRC'd text.
flip a few bits and prove that the result is the same as it was. Teaches CRC
and xor.

### ? word search interview question, with massive hints on how to do it ?
### ? Use other interesting interview questions, with massive hints?
### ? Use problems from algorithm design textbooks?


# Architecture Idea

Instead of a run.sh script, which does all the work and magically compiles and
runs things, instead, give the user a build command (depending on the platform)
and make them build things in their own terminal. This will help them learn
about compiler, compile errors, how to run the program, etc.

Then, I will request that they submit their program to my C-based "checker"
program (with some basic checks to make sure they didn't submit a .c file and
that the program is executable). My checker code will be contained in a single C
file, so it will build quickly and the build command will be a simple one-liner
that they can run themselves.

For each exercise, I will give them a single build command that they will copy
and paste to run themselves. (Mention ctrl+shift+C or cmd+C shortcuts.)

The checker program will have a simple hidden file that keeps track of state,
and that state can be cleared. It can be an append-only database text file, with
the following format, separated by spaces:

```
<exercise_number>_<attempt_number>_<1 for pass,0 for fail>
```

When running the checker with no arguments, it will
* tell the user what file to edit (what folder to open in VSCode)
* print out the build command for the exercise
* print out the command to run the exercise
* print out the command to run the checker on the binary
* If it passes, repeat steps for next exercise
* If that was the last exercise, print completion message.
* If the test fails, give encouragement and tell them to read the readme for
  info.

