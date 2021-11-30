#!/bin/bash




#
# run experiments
#

RUN_PERF_EXPERIMENT=$HOME/debloat/pin_probe_debloat_experiments/nginx/performance/time_variation/run_perf_experiment.sh

num_sec_arr=(
    3
    30
    300
)
pin_nopin_arr=(
    pin
    nopin
)


for x in {1..3}; do
    for num_sec in ${num_sec_arr[@]}; do
        for pnp in ${pin_nopin_arr[@]}; do
            $RUN_PERF_EXPERIMENT $num_sec $pnp
        done
    done
    mkdir -p $x
    mv *.out $x
done





#
# parse results
#
wrk_results=(
    wrk.3s.nopin.out
    wrk.3s.pin.out
    wrk.30s.nopin.out
    wrk.30s.pin.out
    wrk.300s.nopin.out
    wrk.300s.pin.out
)

function get_avg() {
    pin_nopin=$1
    s=$2
    sum=0
    for i in {1..3}; do
        f=$i/wrk.${s}s.${pin_nopin}.out
        transfer_per_sec=$(grep "Transfer/sec" $f | awk '{print $2}')
        transfer_per_sec=${transfer_per_sec::-2}
        sum=`echo $sum + $transfer_per_sec | bc`
    done
    avg=`echo $sum / 3.0 | bc -l`
    echo $avg
}


for s in {3,30,300}; do

    pin_avg=`get_avg "pin" $s`
    nopin_avg=`get_avg "nopin" $s`
    slowdown=`echo $pin_avg / $nopin_avg | bc -l`
    echo "avg slowdown for ${s}s: $slowdown"

done


