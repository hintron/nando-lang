# Nando Lang

This repo contains a set of beginning exercises and tasks to help you learn the
C programming language and basic CPU architecture.

Exercises:

* [Nando Lang](nando-lang/readme.md) Build a very, very simple language
  interpreter (The original task that this repo is named after).


# TODO:

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

* My first floating point inaccuracies: Write a program that accumulates
  floating point inaccuracies. Also, do the same thing in Python and Javascript,
  and point out that everything uses IEEE floating point under the hood. Maybe
  even inspect some assembly? Teaches floating point.

* String shenanigans - Create chars on the stack, and create an array on the
  stack above the chars, and then access the array out of bounds to change the
  chars and to print a string. Teaches arrays, strings, memory, pointers, data
  types.

* Treasure hunt: Follow a set of instructions to bit manipulate some garbled
  data and extract a secret message. This will teach bitwise operations,
  masking, data types, casting, etc.

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
