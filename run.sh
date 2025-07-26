#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

BUILD_TARGET="main"
IS_DEV="false"
CLEAN="false"
RESET="false"
while [[ $# -gt 0 ]]; do
    case "$1" in
    --help|-h)
        echo "run.sh: exercise builder and runner for nando-lang"
        echo ""
        echo "This script builds and runs the C exercises. The first time it runs,"
        echo "it will initialize the exercises by deleting the solutions."
        echo ""
        echo "  --help, -h   Show this help message"
        echo "  --clean      Delete executables and exit."
        echo "  --reset      Reset progress and force a rebuild and rerun of all exercises."
        exit 0
        ;;
    --clean)
        CLEAN="true"
        shift
        ;;
    --reset)
        RESET="true"
        shift
        ;;
    # Undocumented: This skips the init step and builds and runs exercise
    # solutions instead of exercises. For solution development.
    --dev)
        BUILD_TARGET="solution"
        IS_DEV="true"
        shift
        ;;
    *)
        shift
        ;;
    esac
done


if [ "$CLEAN" = "true" ]; then
    echo "Deleting all .bin files..."
    find "$SCRIPT_DIR/exercises" -name "*.bin" -type f -delete
    # Delete all solution files
    find "$SCRIPT_DIR/exercises" -name "solution.c" -type f -delete
elif [ "$IS_DEV" = "false" ] && [ -f "$SCRIPT_DIR/exercises/00-hello-world/solution.c" ]; then
    echo "Initializing exercises..."
    # Deleting all .bin files
    find "$SCRIPT_DIR/exercises" -name "*.bin" -type f -delete
    # Deleting all solutions to the exercises to avoid temptation
    find "$SCRIPT_DIR/exercises" -name "solution.c" -type f -delete
    # Making a copy of every main.c as original.c
    find "$SCRIPT_DIR/exercises" -name "main.c" -type f -exec sh -c 'cp "$1" "$(dirname "$1")/original.c"' _ {} \;

    # Now, print out instructions for how to run the exercises
    echo "Welcome to nando-lang C exercises!"
    echo "************************************************************************"
    echo "To run the exercises, use the following command:"
    echo "  ./run.sh"
    echo ""
    echo "The first exercise will fail, because you haven't fixed it yet!"
    echo ""
    echo "Edit exercise file 00-hello-world/main.c and try compiling and running"
    echo "it until it passes. Then move on to exercise 01 and repeat."
    echo ""
    echo "If you mess up your file, you can always refer to the starting point of original.c."
    echo "If you get stumped and need to look at the solution, visit"
    echo "https://github.com/hintron/nando-lang and view the solution files there."
    echo ""
    echo "Good luck! Remember, the goal is to learn and have fun with C programming!"
    echo ""
    echo "(Avoid the temptation to look at solutions or to use AI - these exercises"
    echo "are designed to help you learn by doing, so try to solve them on your own,"
    echo "and type them out manually.)"
    echo ""
else
    if [ "$RESET" = "true" ]; then
        echo "Resetting progress and forcing rebuild and rerun of all exercises"
        find "$SCRIPT_DIR/exercises" -name ".passed*" -type f -delete
    fi

    declare -A expected_outputs

    # Use arrays for each key to hold multiple expected outputs
    expected_outputs["00-hello-world[0]"]="Hello, world!"
    expected_outputs["01-my-first-segfault[0]"]="x addr: 0x"
    expected_outputs["01-my-first-segfault[1]"]="x val: 1337"
    expected_outputs["01-my-first-segfault[2]"]="y addr: 0x"
    expected_outputs["01-my-first-segfault[3]"]="y val: 3773"

    # Run each exercise
    BUILD_FILE="$BUILD_TARGET.c"
    BUILD_OUTPUT="$BUILD_TARGET.bin"
    for dir in "$SCRIPT_DIR/exercises"/*/; do
        if [ ! -d "$dir" ]; then
            continue
        fi

        dir_key=${dir%/}

        if [ "$IS_DEV" = "true" ]; then
            DEV_FLAG=" --dev"
        else
            DEV_FLAG=""
        fi
        PASSED_FILE="$dir/.passed-$BUILD_TARGET"
        if [ -f "$PASSED_FILE" ]; then
            echo "✅ Exercise $dir_key: Already passed! (run \`./run.sh${DEV_FLAG} --reset\` to rerun)"
            continue
        fi

        echo "*********************************************************************"
        echo "* Exercise $dir_key"
        echo "*********************************************************************"

        BUILD_FILE_FULL="$dir_key/$BUILD_FILE"
        EXECUTABLE_FULL="$dir_key/$BUILD_OUTPUT"
        echo "Build command 🏗️"
        echo "    gcc $BUILD_FILE_FULL -o $EXECUTABLE_FULL"
        if ! gcc "$BUILD_FILE_FULL" -o "$EXECUTABLE_FULL"; then
            echo "❌ Error: Failed to build. Please fix build failures and complete the task."
            exit 1
        fi
        cd "$dir" || exit

        if [ ! -f "$EXECUTABLE_FULL" ]; then
            echo "‼️ Error: $EXECUTABLE_FULL not found in $dir; cannot run exercise."
            exit 1
        fi
        echo "Run command ⏩"
        echo "    $EXECUTABLE_FULL"

        echo "Output:"
        echo ""
        # Capture output and return code
        output=$("$EXECUTABLE_FULL" 2>&1)
        return_code=$?
        echo "$output"
        # Print the output to user
        echo ""
        echo "Return Code: $return_code"

        # Check expected return code if defined
        if [[ $return_code -ne 0 ]]; then
            echo "❌ Failure: Expected return code 0; got return code $return_code"
            exit 1
        fi

        # Iterate over all expected output strings for this test
        # Iterate through all expected outputs for this key
        idx=0
        while true; do
            val="${expected_outputs["$dir_key[$idx]"]}"
            if [ -z "$val" ]; then
                break
            fi
            if ! echo "$output" | grep -Fq -- "$val"; then
                echo "❌ Failure: '$val' not found in output"
                exit 1
            fi
            idx=$((idx + 1))
        done

        echo "✅ Exercise passed!"
        touch "$PASSED_FILE"  # Mark as passed
        cd ..
    done

    echo "🎇🎆🎉🎊🎇🎆🎉🎊🎇🎆🎉🎊🎇🎆🎉🎊🎇🎆🎉🎊🎇🎆🎉🎊🎇🎆🎉🎊🎇🎆🎉🎊"
    echo "Congratulations! 🎉 All exercises have passed."
    echo "Thank you for solving nando-lang! I hope you learned C better."
    echo "Please leave a star on the GitHub repository if you enjoyed it!"
fi