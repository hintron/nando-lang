#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

echo "Deleting all main.bin files"
find "$SCRIPT_DIR" -name "main.bin" -type f -delete
