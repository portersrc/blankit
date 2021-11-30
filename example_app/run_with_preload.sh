#!/bin/bash

#LD_LIBRARY_PATH=pin ./testexe 2
#LD_LIBRARY_PATH=pin ./testexe 100000 # roughly 4.8s

#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/rudy/debloat/pin_probe_debloat_experiments/examples/pin:/home/rudy/debloat/pin_probe_debloat_experiments/examples/shim

pin/mark_writable libs/libpin_normal/libpin.so
pin/mark_writable libs/libc/libc-2.23.so

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/rudy/debloat/pin_probe_debloat_experiments/example_app/libs/libpin_normal:/home/rudy/debloat/pin_probe_debloat_experiments/example_app/shim LD_PRELOAD=/home/rudy/debloat/pin_probe_debloat_experiments/example_app/shim/shim.so ./example_app 2
#LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/rudy/debloat/pin_probe_debloat_experiments/example_app/libs/libpin_normal:/home/rudy/debloat/pin_probe_debloat_experiments/example_app/shim LD_PRELOAD=/home/rudy/debloat/pin_probe_debloat_experiments/example_app/shim/shim.so gdb ./example_app 2
