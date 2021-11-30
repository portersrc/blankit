#!/bin/bash

if [ $# != 1 ]; then
    echo
    echo "Usage:"
    echo "  \$ $0 <path/to/lib.so>"
    echo
    echo "Warning: This permanently overwrites the lib's .text section with zeros"
    echo
    exit 1
fi
# get offset and size of text section
offset_size=$(objdump -h $1 | grep ".text" | awk '{print $4" "$3}')

./zero_out_file_part $1 $offset_size
