#!/bin/bash
set -e


pin/mark_writable libs/libpin/libpin.so
pin/mark_writable libs/libc/libc-2.23.so

# XXX Probably a good idea to use the full path in LD_PRELOAD:
#   https://stackoverflow.com/questions/426230/what-is-the-ld-preload-trick
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/rudy/debloat/pin_probe_debloat_experiments/prep_basicmath/libs/libpin:/home/rudy/debloat/pin_probe_debloat_experiments/prep_basicmath/thunk LD_PRELOAD=/home/rudy/debloat/pin_probe_debloat_experiments/prep_basicmath/thunk/thunk.so ./prep_basicmath 2
#LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/rudy/debloat/pin_probe_debloat_experiments/prep_basicmath/libs/libpin:/home/rudy/debloat/pin_probe_debloat_experiments/prep_basicmath/thunk LD_PRELOAD=/home/rudy/debloat/pin_probe_debloat_experiments/prep_basicmath/thunk/thunk.so gdb ./prep_basicmath 2
