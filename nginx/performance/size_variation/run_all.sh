#!/bin/bash

RUN_PERF_EXPERIMENT=/home/rudy/debloat/pin_probe_debloat_experiments/nginx/performance/size_variation/run_perf_experiment.sh
# XXX note to future self: should probably not drive run_docker_perf_experiment
# with this script. The docker version doesn't restart nginx inbetween runs.
# This script assumes nginx is restarted, so to use it, it requires commenting
# out pin/nopin appropriately, etc. etc.
# Can do something like this to get one set, though (assumes folders exist for the `mv`):
#   for i in {1,2,3}; do for j in {1,10,100}; do ./run_docker_perf_experiment.sh $j pin; mv *.out ${i}-perf; done; done
# Then repeat for nopin, after restarting nginx manually


req_size_arr=(
    1
    10
    100
)
pin_nopin_arr=(
    pin
    nopin
)

for req_size in ${req_size_arr[@]}; do
    for pnp in ${pin_nopin_arr[@]}; do
        $RUN_PERF_EXPERIMENT $req_size $pnp
    done
done
