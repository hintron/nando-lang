#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Define expected outputs and return codes for each exercise
declare -A expected_outputs
declare -A expected_return_codes

expected_outputs["00-hello-world"]="Hello, world!"
expected_return_codes["00-hello-world"]=0
expected_return_codes["01-my-first-segfault"]=139

# Run each exercise
for dir in "$SCRIPT_DIR/exercises"/*/; do
    if [ ! -d "$dir" ]; then
        continue
    fi

    dir_key=${dir%/}

    echo "*********************************************************************"
    echo "Exercise $dir_key"
    echo "*********************************************************************"
    echo "Building $dir_key/main.c"
    if ! gcc "$dir_key/main.c" -o "$dir_key/main.bin"; then
        echo "Error: Failed to build. Please fix build failures and complete the task."
        exit 1
    fi
    echo "Running $dir_key/main.bin"
    cd "$dir" || exit

    if [ ! -f "main.bin" ]; then
        echo "Error: main.bin not found in $dir. Please run build.sh first."
        exit 1
    fi

    echo "Output:"
    echo "---"
    # Capture output and return code
    # output=$(./main.bin 2>&1 | tee /tmp/exercise_output.tmp)
    output=$(./main.bin 2>&1)
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