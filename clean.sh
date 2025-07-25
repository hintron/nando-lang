#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )


DELETE_SOLUTIONS="false"
CLEAR_PROGRESS="false"
while [[ $# -gt 0 ]]; do
    case "$1" in
        --solution|-s)
            DELETE_SOLUTIONS="true"
            shift
            ;;
        --clear|-c)
            CLEAR_PROGRESS="true"
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [--solution|-s] [--help|-h]"
            echo "  --solution, -s   Delete all solution files, to avoid temptation."
            echo "  --clear, -c      Clear all progress files, so when you run the exercises again,"
            echo "                   they will be built and run fresh."
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
fi

if [ "$CLEAR_PROGRESS" = "true" ]; then
    echo "Clearing all progress files"
    find "$SCRIPT_DIR/exercises" -name ".passed*" -type f -delete
fi