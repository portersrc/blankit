#!/bin/bash
#set -x

WRK=/home/rudy/debloat/wrk/wrk
DRIVE_NGINX=/home/rudy/debloat/pin_probe_debloat_experiments/drive_nginx.sh


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

echo
echo "Should be running this script as root"
echo


echo "Assuming nginx is already running in a container..."
ps aux | grep nginx
sleep 1


nginx_perf_out=nginx-perf.${req_size}mb.${pin_nopin}.out

echo "Attaching perf to nginx worker process"
echo "writing to $nginx_perf_out"
perf stat -p $(ps aux | grep "nginx: worker process" | head -n1 | awk '{print $2}') &> $nginx_perf_out &


echo "Running wrk (${req_size}mb)"
wrk_out=wrk.${req_size}mb.${pin_nopin}.out
$WRK -t12 -c400 -d30s http://127.0.0.1:8080/${req_size}mb.bin &> $wrk_out

echo
echo "Killing perf"
perf_pid=$(ps aux | grep "perf stat -p" | head -n1 | awk '{print $2}')
kill -2 ${perf_pid}



echo
echo "RESULT:"
echo "======="
cat $wrk_out

echo
echo "PERF RESULT:"
echo "==========="
cat $nginx_perf_out
