#!/bin/bash
ps -aux | grep nginx ; pkill nginx ; ps -aux | grep nginx
./drive_nginx.sh &> outch
pushd /home/rudy/debloat/wrk
./test.sh 3
popd
