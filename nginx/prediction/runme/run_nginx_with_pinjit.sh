#!/bin/bash



#$PINTOOL -t $PIN/proccount.so -- $line  | $TOOL/parsePinTrace  &>> ${buildDIR}/debloat_log

PIN_ROOT=/home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux
NGINX_ROOT=/home/rudy/debloat/nginx

BENCHMARK=$NGINX_ROOT/objs/nginx_instrumentpin
#BENCHMARK_ARGS="-c $NGINX_ROOT/conf/nginx.conf -p $NGINX_ROOT"
BENCHMARK_ARGS="-c $NGINX_ROOT/conf/nginx.conf -p $NGINX_ROOT -g 'daemon off;'"



# Note: This doesn't work with 'daemon off' option.
#$PIN_ROOT/pin -t \
#  $PIN_ROOT/source/tools/ManualExamples/obj-intel64/proccount.so \
#  -- $BENCHMARK $BENCHMARK_ARGS

# hard-coded variables in order to use daemon off and still get output.
# should be a better way to do this.
$PIN_ROOT/pin -t $PIN_ROOT/source/tools/ManualExamples/obj-intel64/proccount.so -- /home/rudy/debloat/nginx/objs/nginx_instrumentpin -c /home/rudy/debloat/nginx/conf/nginx.conf -p /home/rudy/debloat/nginx -g 'daemon off;'

