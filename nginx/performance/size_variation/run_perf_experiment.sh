#!/bin/bash
#set -x

WRK=$HOME/debloat/wrk/wrk
DRIVE_NGINX=$HOME/debloat/pin_probe_debloat_experiments/drive_nginx.sh


function usage_and_exit() {
    echo
    echo "Usage:"
    echo "  $0 [1, 10, 100] [pin, nopin]"
    echo
    exit 1
}

if [ "$1" != "1" ] && [ "$1" != "10" ] && [ "$1" != "100" ]; then
    usage_and_exit
fi

if [ "$2" != "pin" ] && [ "$2" != "nopin" ]; then
    usage_and_exit
fi

req_size=$1
pin_nopin=$2


echo "Ensuring nginx is not running"
pkill nginx
ps -au | grep nginx
sleep 1

echo "Starting nginx"
nginx_out=nginx.${req_size}mb.${pin_nopin}.out
$DRIVE_NGINX $pin_nopin &> $nginx_out
sleep 20


echo "Running wrk (${req_size}mb)"
wrk_out=wrk.${req_size}mb.${pin_nopin}.out
$WRK -t12 -c400 -d30s http://127.0.0.1:8080/${req_size}mb.bin &> $wrk_out

echo "Tearing down nginx"
pkill nginx
ps -au | grep nginx


echo
echo "RESULT:"
echo "======="
cat $wrk_out
