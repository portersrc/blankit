#!/bin/bash
set -x

#
# XXX
# debloat/nginx/conf/nginx.conf needed "user root root;", and this probably
# breaks usage now directly in the host.
#

PIN_ROOT=$HOME/else/pin-3.6-97554-g31f0a167d-gcc-linux
NGINX_ROOT=$HOME/debloat/nginx
BLANKIT_NGINX_ROOT=$HOME/debloat/pin_probe_debloat_experiments/nginx

#BENCHMARK=$NGINX_ROOT/objs/nginx
#BENCHMARK=$NGINX_ROOT/objs/nginx_instrumentpin
BENCHMARK=$BLANKIT_NGINX_ROOT/bin/nginx
#BENCHMARK=$BLANKIT_NGINX_ROOT/bin/nginx_instrumentpin
BENCHMARK_AUDIT=$BLANKIT_NGINX_ROOT/bin/nginx_audit

BENCHMARK_ARGS="-c $NGINX_ROOT/conf/nginx.conf -p $NGINX_ROOT"
#BENCHMARK_ARGS="-c $NGINX_ROOT/conf/nginx.conf -p $NGINX_ROOT -g 'daemon off;'"

export BLANKIT_APP_NAME=$NGINX_ROOT/$BENCHMARK
export BLANKIT_APPROVED_LIST=$BLANKIT_NGINX_ROOT/approved_list.txt
export BLANKIT_APPROVED_LIST_LIBDL=$BLANKIT_NGINX_ROOT/other-approved-lists/approved_list_libdl.txt
export BLANKIT_APPROVED_LIST_LIBPTHREAD=$BLANKIT_NGINX_ROOT/other-approved-lists/approved_list_libpthread.txt
export BLANKIT_APPROVED_LIST_LIBCRYPT=$BLANKIT_NGINX_ROOT/other-approved-lists/approved_list_libcrypt.txt
export BLANKIT_APPROVED_LIST_LIBPCRE=$BLANKIT_NGINX_ROOT/other-approved-lists/approved_list_libpcre.txt
export BLANKIT_APPROVED_LIST_LIBZ=$BLANKIT_NGINX_ROOT/other-approved-lists/approved_list_libz.txt

# FIXME
#export BLANKIT_PREDICT_SETS=$BENCHMARKS_ROOT/$BENCHMARK_FOLDER/string2id_map.csv

#export BLANKIT_TRACE=1

if [ "$1" == "nopin" ]; then
    echo "Running WITHOUT pin"
    $BENCHMARK $BENCHMARK_ARGS

    # eval version, for reference (works with  daemon off BENCHMARK_ARGS)
    #eval "$BENCHMARK $BENCHMARK_ARGS"

elif [ "$1" == "audit" ]; then
    echo "Running AUDIT"
    #LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/rudy/debloat/pathPredictionProject/src \
    #               $BENCHMARK_AUDIT $BENCHMARK_ARGS

    # Hard-coded daemon-off to avoid nginx invalid option
    LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/rudy/debloat/pathPredictionProject/src \
      /home/rudy/debloat/pin_probe_debloat_experiments/nginx/bin/nginx_audit -c /home/rudy/debloat/nginx/conf/nginx.conf -p /home/rudy/debloat/nginx -g 'daemon off;'


else
    echo "Running with pin"
    $PIN_ROOT/pin -t $PIN_ROOT/source/tools/Probes/obj-intel64/decrypt_probe.so -- $BENCHMARK $BENCHMARK_ARGS

    # DON'T USE. This was intended for use this with BENCHMARK_ARGS with daemon
    # off. Leverages eval, but not sure how to get the output.
    #$PIN_ROOT/pin -t $PIN_ROOT/source/tools/Probes/obj-intel64/decrypt_probe.so -- $(eval "$BENCHMARK $BENCHMARK_ARGS")

    # Use this with BENCHMARK_ARGS with daemon off (hard-coded variables, works
    # with output)... gotta be a better way to do this.
    #$PIN_ROOT/pin -t $PIN_ROOT/source/tools/Probes/obj-intel64/decrypt_probe.so -- /home/rudy/debloat/pin_probe_debloat_experiments/nginx/bin/nginx -c /home/rudy/debloat/nginx/conf/nginx.conf -p /home/rudy/debloat/nginx -g 'daemon off;'
fi

