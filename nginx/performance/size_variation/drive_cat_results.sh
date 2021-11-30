#!/bin/bash

wrk_results=(
    wrk.1mb.nopin.out
    wrk.1mb.pin.out
    wrk.10mb.nopin.out
    wrk.10mb.pin.out
    wrk.100mb.nopin.out
    wrk.100mb.pin.out
)

for i in {1..3}; do

    for wrk_result in ${wrk_results[@]}; do
        f=$i/$wrk_result
        echo $f
        ./cat_results.sh $f
        echo
    done
    echo

done
