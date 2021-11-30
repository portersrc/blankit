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

echo "benchmark  num-exposed-funcs  num-partially-exposed-funcs  %-glibc-cve-reduction"
echo "---------  -----------------  ---------------------------  ---------------------"
for b in ${benchmarks[@]}; do
    num_exposed_and_partially_exposed=$(./cve_report.py $b)
    sum=0
    for n in $num_exposed_and_partially_exposed; do
         sum=$((sum + $n))
    done

    printf "$b $num_exposed_and_partially_exposed "
    echo "(47-$sum)/47 * 100" | bc -l
done
