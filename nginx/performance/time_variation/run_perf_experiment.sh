#!/bin/bash
#set -x

WRK=$HOME/debloat/wrk/wrk
DRIVE_NGINX=$HOME/debloat/pin_probe_debloat_experiments/drive_nginx.sh


function usage_and_exit() {
    echo
    echo "Usage:"
    echo "  $0 [3, 30, 300] [pin, nopin, audit]"
    echo
    exit 1
}

if [ "$1" != "3" ] && [ "$1" != "30" ] && [ "$1" != "300" ]; then
    usage_and_exit
fi

if [ "$2" != "pin" ] && [ "$2" != "nopin" ] && [ "$2" != "audit" ]; then
    usage_and_exit
fi

num_sec=$1
pin_nopin_audit=$2


echo "Ensuring nginx is not running"
pkill nginx
ps aux | grep nginx
sleep 1

echo "Starting nginx"
nginx_out=nginx.${num_sec}s.${pin_nopin_audit}.out
$DRIVE_NGINX $pin_nopin_audit &> $nginx_out &
sleep 20 # wait to make sure it got kicked off completely in the background


echo "Running wrk ($num_sec seconds)"
wrk_out=wrk.${num_sec}s.${pin_nopin_audit}.out
$WRK -t12 -c400 -d${num_sec}s http://127.0.0.1:8080/wikipedia_main_page.html &> $wrk_out
#$WRK -t12 -c400 -d${num_sec}s http://127.0.0.1:8080/1mb.bin &> $wrk_out

echo "Tearing down nginx"
pkill nginx
ps aux | grep nginx


echo
echo "RESULT:"
echo "======="
cat $wrk_out
