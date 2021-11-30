#!/bin/bash
set -x
set -e

#LD_LIBRARY_PATH=pin ./testexe 2
#LD_LIBRARY_PATH=pin ./testexe 100000 # roughly 4.8s
pushd /home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/Probes
make obj-intel64/decrypt_probe.so
popd

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:libs/libpin_normal:../blankit

/home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux/pin -t \
  /home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/Probes/obj-intel64/decrypt_probe.so \
  -- ./example_app 2

