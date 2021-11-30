#!/bin/bash
set -x
set -e

pushd /home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/Probes
make obj-intel64/decrypt_probe.so
popd

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../../blankit
export BLANKIT_APP_NAME=/home/rudy/debloat/pin_probe_debloat_experiments/susan/pri-sus/susan
export BLANKIT_APPROVED_LIST=/home/rudy/debloat/pin_probe_debloat_experiments/susan/pri-sus/approved_list.txt

#/home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux/pin -ifeellucky -t \
#  /home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/Probes/obj-intel64/decrypt_probe.so \
#  -- ./susan input_small.pgm output_small.smoothing.pgm -s
/home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux/pin -t \
  /home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/Probes/obj-intel64/decrypt_probe.so \
  -- ./susan input_small.pgm output_small.smoothing.pgm -s
