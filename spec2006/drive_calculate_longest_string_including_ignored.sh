#!/bin/bash

set -e

benchmarks=(
    401.bzip2
    403.gcc
    429.mcf
    433.milc
    444.namd
    445.gobmk
    450.soplex
    453.povray
    456.hmmer
    458.sjeng
    462.libquantum
    464.h264ref
    470.lbm
    471.omnetpp
    473.astar
    482.sphinx3
    483.xalancbmk
)

for b in ${benchmarks[@]}; do
    #echo $b
    #./calculate_longest_string_including_ignored.py $b
    num_exposed_funcs=$(./calculate_longest_string_including_ignored.py $b)
    echo $b $num_exposed_funcs
done
