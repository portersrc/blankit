#!/bin/bash

#set -e # probably dont want this. not all benchmarks have same libs, and it
        # is easier to just run for all of them and afterward ignore errors
        # than to meticulously filter which we run.
#set -x


function usage() {
    echo
    echo "Usage:"
    echo "  $0 [libc, libm, libgcc, libstdcpp]"
    echo
    exit 1
}




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

if [ $# != 1 ]; then
	usage
fi

lib=$1

for b in ${benchmarks[@]}; do
    ./get_ignored_list.py $b $lib > $b.ignored_list.txt
done
