#!/bin/bash

if [ $# != 1 ]; then
    echo
    echo "Usage:"
    echo "  $0 <path/to/lib.so>"
    echo
    exit 1
fi
objdump -h $1
