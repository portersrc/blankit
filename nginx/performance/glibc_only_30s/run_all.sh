#!/bin/bash

RUN_PERF_EXPERIMENT=/home/rudy/debloat/pin_probe_debloat_experiments/nginx/performance/glibc_only_30s/run_perf_experiment.sh

echo
echo "IMPORTANT: For this result to be valid, the image_load() needs to be turned off for all extra libs but glibc, and then the pintool needs to be recompiled."
echo

num_sec_arr=(
    30
)
pin_nopin_arr=(
    pin
)

for num_sec in ${num_sec_arr[@]}; do
    for pnp in ${pin_nopin_arr[@]}; do
        $RUN_PERF_EXPERIMENT $num_sec $pnp
    done
done
