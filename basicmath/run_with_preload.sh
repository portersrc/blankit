#!/bin/bash
set -e

# XXX this needs to be done in general, but we're leaving an already modified
# libc in this folder, so no need here.
#pin/mark_writable libs/libc/libc-2.23.so

# XXX Probably a good idea to use the full path in LD_PRELOAD:
#   https://stackoverflow.com/questions/426230/what-is-the-ld-preload-trick

# when working with ldpreload... this attempted to pass an extra string for strlen purposes
#LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/rudy/debloat/pin_probe_debloat_experiments/basicmath/thunk LD_PRELOAD=/home/rudy/debloat/pin_probe_debloat_experiments/basicmath/thunk/thunk.so ./basicmath_small helloyou

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/rudy/debloat/pin_probe_debloat_experiments/basicmath/thunk LD_PRELOAD=/home/rudy/debloat/pin_probe_debloat_experiments/basicmath/thunk/thunk.so ./basicmath_small
