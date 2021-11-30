#!/bin/bash

set -e
set -x


function usage() {
    echo
    echo "Usage:"
    echo "  $0 [raw, blankit]"
    echo
    exit 1
}

function capture_all_aux() {
    for i in {1..3}; do
        mkdir $i
        ./capture_all.sh times ref $1
        mv *_results *_results.times $i
    done
}

if [ $# == 1 ]; then
    if [ $1 == "raw" ] || [ $1 == "blankit" ]; then
        capture_all_aux $1
        exit 0
    fi
fi
usage
