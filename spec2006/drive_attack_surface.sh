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

echo "benchmark  num-exposed-funcs  %-exposed-code-surface-reduction"
echo "---------  -----------------  --------------------------------"
for b in ${benchmarks[@]}; do
    num_exposed_funcs_and_percent_reduction=$(./attack_surface.py $b)
    echo $b $num_exposed_funcs_and_percent_reduction
done
