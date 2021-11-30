#!/bin/bash
set -x
set -e
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../../blankit
/home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux/pin -t \
  /home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/Probes/obj-intel64/decrypt_probe.so \
  -- ./susan input_small.pgm output_small.smoothing.pgm -s
