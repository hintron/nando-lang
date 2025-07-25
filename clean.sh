#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )


DELETE_SOLUTIONS="false"
while [[ $# -gt 0 ]]; do
    case "$1" in
        --solution|-s)
            DELETE_SOLUTIONS="true"
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [--solution|-s] [--help|-h]"
            echo "  --solution, -s   Delete all solution files, to avoid temptation."
            echo "  --help, -h       Show this help message"
            exit 0
            ;;
        *)
            shift
            ;;
    esac
done


echo "Deleting all .bin files"
find "$SCRIPT_DIR/exercises" -name "*.bin" -type f -delete

if [ "$DELETE_SOLUTIONS" = "true" ]; then
    echo "Deleting all solution files"
    find "$SCRIPT_DIR/exercises" -name "solution.c" -type f -delete
else
    echo "Skipping deletion of solution files"
fi