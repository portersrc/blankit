#!/bin/bash

set -e
#set -x


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
    cp $b/approved_list.txt ../v3-approved-lists/$b.approved_list.txt
done
