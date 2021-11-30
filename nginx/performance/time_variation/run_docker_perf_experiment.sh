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

echo
echo "Should be running this script as root"
echo

echo "Assuming nginx is already running in a container..."
ps aux | grep nginx
sleep 1

nginx_perf_out=nginx-perf.${num_sec}s.${pin_nopin_audit}.out


echo "Attaching perf to nginx worker process"
echo "writing to $nginx_perf_out"
perf stat -p $(ps aux | grep "nginx: worker process" | head -n1 | awk '{print $2}') &> $nginx_perf_out &


echo "Running wrk ($num_sec seconds)"
wrk_out=wrk.${num_sec}s.${pin_nopin_audit}.out
$WRK -t12 -c400 -d${num_sec}s http://127.0.0.1:8080/wikipedia_main_page.html &> $wrk_out
#$WRK -t12 -c400 -d${num_sec}s http://127.0.0.1:8080/1mb.bin &> $wrk_out

echo
echo "Killing perf"
perf_pid=$(ps aux | grep "perf stat -p" | head -n1 | awk '{print $2}')
kill -2 ${perf_pid}



echo
echo "WRK RESULT:"
echo "==========="
cat $wrk_out

echo
echo "PERF RESULT:"
echo "==========="
cat $nginx_perf_out
