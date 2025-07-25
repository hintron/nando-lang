#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

BUILD_TARGET="main"
while [[ $# -gt 0 ]]; do
    case "$1" in
        --solution|-s)
            BUILD_TARGET="solution"
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [--solution|-s] [--help|-h]"
            echo "  --solution, -s   Build and run exercise solutions instead of exercises. Solutions"
            echo "                   should all be compilable and pass all tests."
            echo "  --help, -h       Show this help message"
            exit 0
            ;;
        *)
            shift
            ;;
    esac
done

# Define expected outputs and return codes for each exercise
declare -A expected_outputs
declare -A expected_return_codes

expected_outputs["00-hello-world"]="Hello, world!"
expected_return_codes["00-hello-world"]=0
expected_return_codes["01-my-first-segfault"]=139

# Run each exercise
BUILD_FILE="$BUILD_TARGET.c"
BUILD_OUTPUT="$BUILD_TARGET.bin"
for dir in "$SCRIPT_DIR/exercises"/*/; do
    if [ ! -d "$dir" ]; then
        continue
    fi

    dir_key=${dir%/}

    echo "*********************************************************************"
    echo "Exercise $dir_key"
    echo "*********************************************************************"
    BUILD_FILE_FULL="$dir_key/$BUILD_FILE"
    BUILD_OUTPUT_FULL="$dir_key/$BUILD_OUTPUT"
    echo "Building $BUILD_FILE_FULL"
    if ! gcc "$BUILD_FILE_FULL" -o "$BUILD_OUTPUT_FULL"; then
        echo "Error: Failed to build. Please fix build failures and complete the task."
        exit 1
    fi
    echo "Running $BUILD_OUTPUT_FULL"
    cd "$dir" || exit

    if [ ! -f "$BUILD_OUTPUT_FULL" ]; then
        echo "Error: $BUILD_OUTPUT_FULL not found in $dir; cannot run exercise."
        exit 1
    fi

    echo "Output:"
    echo "---"
    # Capture output and return code
    # output=$(./main.bin 2>&1 | tee /tmp/exercise_output.tmp)
    output=$("$BUILD_OUTPUT_FULL" 2>&1)
    return_code=$?
    echo "$output"
    # output=$(cat /tmp/exercise_output.tmp)
    # Print the output to user
    echo "---"
    echo "Return Code: $return_code"


    # Check expected output if defined
    if [[ -n "${expected_outputs[$dir_key]}" ]]; then
        if [[ "$output" != *"${expected_outputs[$dir_key]}"* ]]; then
            echo "Error: Expected output '${expected_outputs[$dir_key]}' not found in output"
            exit 1
        fi
    fi

    # Check expected return code if defined
    if [[ -n "${expected_return_codes[$dir_key]}" ]]; then
        if [[ $return_code -ne ${expected_return_codes[$dir_key]} ]]; then
            echo "Error: Exercise failed; Expected return code ${expected_return_codes[$dir_key]}; got return code $return_code"
            exit 1
        fi
    fi

    echo "Exercise completed!"
    cd ..
done

echo "========================================================================="
echo "All exercises completed successfully!"