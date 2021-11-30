#!/bin/bash

wrk_results=(
    wrk.3s.nopin.out
    wrk.3s.pin.out
    wrk.30s.nopin.out
    wrk.30s.pin.out
    wrk.300s.nopin.out
    wrk.300s.pin.out
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
