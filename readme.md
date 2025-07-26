# Nando Lang

This repo contains a set of beginning exercises and tasks to help you learn the
C programming language and basic CPU architecture.


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


# Exercises:

* [Nando Lang](exercises/nando-lang/readme.md) Build a very, very simple language
  interpreter (The original task that this repo is named after).


# TODO:

Make a top-level executable script. Run it first without arguments to initialize
things. Then, subsequent runs will build and run the exercises. Merge init.sh,
clean.sh, build.sh, and run.sh into that single executable.

Create some exercises that will help teach C in a fun way. Support it for MacOS,
Linux (Ubuntu), and WSL (Ubuntu). Have each exercise have a readme page with
explanations, hints, and external resources. Each exercise will also have
bash-based tests for each checkpoint. Only audit certain bits here and there, so
as to not give away the answer.


Exercises to create:

* ? My first compilation error: Write and try to compile hello world

* My first segfault: Write a program that dereferences a null pointer. Also,
  write a program that goes off the end of an array and prints stuff until it
  causes a segfault. Teaches pointers.

* My first stack overflow: Write a program that will infinitely recurse a
  function. Print out how many times it's called, and print the address of
  locals. Calculate the rough size of the stack before it overflows. Then,
  try to allocate a large array on the stack and see how big it can get. Teaches
  recursion, stack limits, memory.

* My first OOM: Write a program that continually allocates memory until you run
  out of memory. Ask the programmer to view the computer memory in real time to
  see what happens. Teaches memory, CPUs, memory management.

* My first integer overflow: Write a program that overflows and underflows an
  integer. Convert between signed and unsigned, and evaluate integer math and
  2's complement. Teaches 2's compliment, integer arithmetic, casting.

* My first divide by 0: Write a program that divides by 0.

* Numbers do *what*?: Write a program that takes two small signed numbers, cast
  them to unsigned, add them together, print the results, then cast them back to
  signed. Also, print the hex at each stage. Show that unsigned and signed
  integer arithmetic is the same, because of two's complement. Under the hood,
  the CPU has integer add/sub instructions that don't care about the sign.
  Also, do multiplication, but notice that the difference is in sign extension
  of the upper bits
  Resources:
  * https://stackoverflow.com/questions/42587607/why-is-imul-used-for-multiplying-unsigned-numbers
  * https://www.computerenhance.com/p/q-and-a-22-2023-08-15?utm_campaign=post&utm_medium=web&timestamp=394

* Gulliver's Travels: Write a program with large 64-bit numbers, and print out
  each byte via masks. Show that the ints are little endian in memory. Could
  also convert to an array of uint8s to show memory contents. Show that
  single-byte values are the same order either way. Have some array of u8s,
  create a u16 with the MSB 1 and LSB 0. Guess the value (256).
  Teaches endianness, byte manipulation, integers, integer ranges, MSB.

* My first floating point inaccuracies: Write a program that accumulates
  floating point inaccuracies. Also, do the same thing in Python and Javascript,
  and point out that everything uses IEEE floating point under the hood. Maybe
  even inspect some assembly? Teaches floating point.

* String shenanigans - Create chars on the stack, and create an array on the
  stack above the chars, and then access the array out of bounds to change the
  chars and to print a string. Teaches arrays, strings, memory, pointers, data
  types.

* Histogram: Process a text file and print a histogram of how many times each
  letter is used. Print to screen.

* Treasure hunt: Follow a set of instructions to bit manipulate some garbled
  data and extract a secret message. This will teach bitwise operations,
  masking, data types, casting, etc.

* Mini REPL: Make a mini repl/shell with some simple commands.

* Image-in that!: Extract an image from garbled data and piece it together,
  like a jigsaw puzzle. Teaches bit manipulation, arrays, file IO.

* My first triangle: Run a simple Raylib window. Then, using arrays and bit
  manipulation, create a colored RGB triangle. Start with colors being discrete,
  then interpolated. Take an image we already worked on and insert part of that
  into the triangle somehow. Teaches bit manipulation, arrays, file IO, graphics.

* Out of this world: Create a mini orbiting solar system using the gravitational
  equation and planet sprites. Spawn planets with a simple click in a given
  direction. Teaches math, simulation, mouse input handling, event loops.

* De Morgan's Lawyer: Reduce some boolean logic in C code to it's most optimal
  form using De Morgan's law and other boolean identities.

* Grand C (race between C vs. Python vs. Javascript): Do a somewhat complicated
  text processing problem in vanilla C, Python, and Javascript, and compare the
  times. How much faster is C? Also, add in print statements throughout, then
  disable them with #ifdefs. How much do print statements slow things down?

* Floating in the C: Write a program that will take a floating point number, put
  it in a union, and then mask out the bits to determine what the mantissa,
  exponent, etc. are. Do it for doubles as well. Multiply the floating point
  number by a value using only bit operations.

* Fix up "Nando lang" to be more high-level (`a = b` instead of `set a b`), and
  implement the low level as "Nando asm".

* Ride the Wav: Do some bit manipulations on a piece of simple audio. Maybe even
  do a convolution of one piece of audio with another piece to apply an 'echo'
  or 'cathedral' effect?

* Cipher Security: Write a program that will use simple ciphers to "encrypt"
  data, and to decrypt it again. Teaches encryption.

* Compress Me: Write a program that will do something like Huffman encoding in
  order to compress a thing of text. See what kind of savings you can get. Be
  able to compress and decompress the same thing. Teaches compression.

* CRC is my BFF: Write a simple program that converts some text into CRC'd text.
  flip a few bits and prove that the result is the same as it was. Teaches CRC
  and xor.

* ? word search interview question, with massive hints on how to do it ?
* ? Use other interesting interview questions, with massive hints?
* ? Use problems from algorithm design textbooks?
