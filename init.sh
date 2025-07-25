#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

echo "Deleting all .bin files."
find "$SCRIPT_DIR/exercises" -name "*.bin" -type f -delete
echo "Deleting all solutions to the exercises to avoid temptation."
find "$SCRIPT_DIR/exercises" -name "solution.c" -type f -delete

echo "Making a copy of every main.c as original.c."
find "$SCRIPT_DIR/exercises" -name "main.c" -type f -exec sh -c 'cp "$1" "$(dirname "$1")/original.c"' _ {} \;

# Print out instructions for how to run the exercises
echo "To run the exercises, use the following command:"
echo "  ./run.sh"
echo ""
echo "The first exercise will fail, because you haven't fixed it yet!"
echo ""
echo "Edit exercise file 00-hello-world/main.c and try compiling and running"
echo "it until it passes. Then move on to exercise 01 and repeat."
echo ""
echo "If you mess up your file, you can always refer to the starting point of backup.c"
echo "If you get stumped and need to look at the solution, visit"
echo "https://github.com/hintron/nando-lang and view the solution files there."
echo ""
echo "Good luck! Remember, the goal is to learn and have fun with C programming!"
echo ""
echo "(Avoid the temptation to look at the solutions or to use AI - these exercises"
echo "are designed to help you learn by doing, so try to solve them on your own"
echo "and type them out manually.)"
echo ""


