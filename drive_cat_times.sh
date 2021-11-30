#!/bin/bash

for i in {1..3}; do
    cd $i
    ../cat_times.sh
    echo
    cd ..
done
