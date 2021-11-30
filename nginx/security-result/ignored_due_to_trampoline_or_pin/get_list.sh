#!/bin/bash
for f in `ls`; do
    echo $f
    grep "RTN name:.*not adding probe" $f | awk '{print $3}'
    echo
    echo
done
