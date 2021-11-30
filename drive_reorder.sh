#!/bin/bash

for i in {1..3}; do
    cd $i
    ../reorder.sh
    cd ..
done
