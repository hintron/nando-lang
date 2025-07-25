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
            echo "  --solution, -s   Build solution.c instead of main.c. Solutions"
            echo "                   should all be compilable and pass all tests."
            echo "  --help, -h       Show this help message"
            exit 0
            ;;
        *)
            shift
            ;;
    esac
done

BUILD_FILE="$BUILD_TARGET.c"
BUILD_OUTPUT="$BUILD_TARGET.bin"

failed="false"
for dir in "$SCRIPT_DIR/exercises"/*/; do
    dir=${dir%/}
    if [ ! -f "$dir/$BUILD_FILE" ]; then
        continue
    fi
    echo "Building $dir/$BUILD_FILE"
    if ! gcc "$dir/$BUILD_FILE" -o "$dir/$BUILD_OUTPUT"; then
        failed="true"
    fi
done

if [ "$failed" = "true" ]; then
    echo "ERROR: One or more builds failed"
    exit 1
fi
