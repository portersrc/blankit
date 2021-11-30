#!/bin/bash
set -x
set -e
export LD_LIBRARY_PATH=/home/rudy/debloat/pin_probe_debloat_experiments/blankit
export BLANKIT_APP_NAME=/home/rudy/debloat/pin_probe_debloat_experiments/basicmath/basicmath_small
export BLANKIT_APPROVED_LIST=/home/rudy/debloat/pin_probe_debloat_experiments/basicmath/approved_list.txt

/home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux/pin -t \
  /home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/Probes/obj-intel64/decrypt_probe.so \
  -- ./basicmath_small
