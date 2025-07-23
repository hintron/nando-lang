#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

failed="false"
for dir in "$SCRIPT_DIR"/*/; do
    dir=${dir%/}
    if [ ! -f "$dir/main.c" ]; then
        continue
    fi
    echo "Building $dir/main.c"
    if ! gcc "$dir/main.c" -o "$dir/main.bin"; then
        failed="true"
    fi
done

if [ "$failed" = "true" ]; then
    echo "ERROR: One or more builds failed"
    exit 1
fi
