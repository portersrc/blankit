#!/bin/bash

# base test case
#./wrk -t12 -c400 -d$1s http://127.0.0.1:8080/index.html &> outch

# 404 test case
#./wrk -t12 -c400 -d$1s http://127.0.0.1:8080/giveme404.html &> outch

# big.txt from peter norvig
#./wrk -t12 -c400 -d$1s http://127.0.0.1:8080/big.txt &> outch

# example grep for quickly getting output numbers
#cat outch | grep "Latency" | awk '{print $2}' | sed 's/..$//'
#cat outch | grep "Req/Sec" | awk '{print $2}' | sed 's/.$//'
#cat outch | grep "Transfer/sec" | awk '{print $2}' | sed 's/..$//'


# XXX: This is based on the nginx site's approach, which has taskset because
# "this method yields more consistent results than increasing the number of wrk
# threads." I didn't find that to be the case. First run with this approach
# yielded thread transfer / sec variation between 491 KB to 2.41 MB.
# Also, if this is the approach, then pin/nginx should be launched with taskset
# as well.
#for i in `seq 1 12`; do
#     taskset -c $i ./wrk -t1 -c400 -d$1s \
#       http://127.0.0.1:8080/index.html &> outch$i &
#done

#wait

WRK=/home/rudy/debloat/wrk/wrk


function usage_and_exit() {
    echo
    echo "Usage:"
    echo "  $0 train"
    echo "  $0 test [wiki, 1mb]"
    echo
    exit 1
}


if [ "$1" == "train" ]; then
    $WRK -t12 -c400 -d3s http://127.0.0.1:8080/site_under_construction.html &> wrk.out
elif [ "$1" == "test" ]; then
  if [ "$2" == "wiki" ]; then
    $WRK -t12 -c400 -d30s http://127.0.0.1:8080/wikipedia_main_page.html &> wrk.out
  elif [ "$2" == "1mb" ]; then
    $WRK -t12 -c400 -d30s http://127.0.0.1:8080/1mb.bin &> wrk.out
  else
      usage_and_exit
  fi
else
    usage_and_exit
fi
