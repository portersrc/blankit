#!/bin/bash
for f in `ls`; do
    printf $f" "
    grep Average $f | awk '{print $4}'
done
