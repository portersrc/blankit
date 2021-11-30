#!/bin/bash

set -x
set -e


function usage() {
    echo
    echo "Usage:"
    echo "  $0 times [train, ref] [raw, blankit]"
    echo "  $0 traces [train, ref]"
    echo "  $0 [audit, auditmem]"
    echo
    exit 1
}

v4_benchmarks=(
    bzip
    gcc
    mcf
    milc
    namd
    gobmk
    soplex
    povray
    hmmer
    sjeng
    libquantum
    h264ref
    lbm
    omnetpp
    astar
    sphinx
    xalancbmk
)


function capture_all_times() {
    for benchmark in ${v4_benchmarks[@]}; do
        results_file=${benchmark}_results
        ./drive_blankit.sh run $benchmark $1 $2 &> $results_file \
          && ./get_times.sh $results_file
    done
}

function capture_all_audit() {
    for benchmark in ${v4_benchmarks[@]}; do
        results_file=${benchmark}_results
        ./drive_blankit.sh run $benchmark ref $1 &> $results_file
    done
}

function capture_all_traces() {
    for benchmark in ${v4_benchmarks[@]}; do
        ./drive_blankit.sh trace $benchmark
    done
}


if [ $# == 1 ]; then
    if [ $1 == "audit" ] || [ $1 == "auditmem" ]; then
        capture_all_audit $1
    else
        usage
    fi
elif [ $# == 2 ] && [ $1 == "traces" ]; then
    capture_all_traces $2
elif [ $# == 3 ] && [ $1 == "times" ]; then
    capture_all_times $2 $3
else
    usage
fi

