#!/bin/bash




#
# run experiments
#

RUN_PERF_EXPERIMENT=$HOME/debloat/pin_probe_debloat_experiments/nginx/performance/size_variation/run_perf_experiment.sh

req_size_arr=(
    1
    10
    100
)
pin_nopin_arr=(
    pin
    nopin
)


for x in {1..3}; do
    for req_size in ${req_size_arr[@]}; do
        for pnp in ${pin_nopin_arr[@]}; do
            $RUN_PERF_EXPERIMENT $req_size $pnp
        done
    done
    mkdir -p $x
    mv *.out $x
done





#
# parse results
#
wrk_results=(
    wrk.1mb.nopin.out
    wrk.1mb.pin.out
    wrk.10mb.nopin.out
    wrk.10mb.pin.out
    wrk.100mb.nopin.out
    wrk.100mb.pin.out
)

function get_avg() {
    pin_nopin=$1
    mb=$2
    sum=0
    for i in {1..3}; do
        f=$i/wrk.${mb}mb.${pin_nopin}.out
        transfer_per_sec=$(grep "Transfer/sec" $f | awk '{print $2}')
        transfer_per_sec=${transfer_per_sec::-2}
        sum=`echo $sum + $transfer_per_sec | bc`
    done
    avg=`echo $sum / 3.0 | bc -l`
    echo $avg
}


for mb in {1,10,100}; do

    pin_avg=`get_avg "pin" $mb`
    nopin_avg=`get_avg "nopin" $mb`
    slowdown=`echo $pin_avg / $nopin_avg | bc -l`
    echo "avg slowdown for ${mb}mb: $slowdown"

done


