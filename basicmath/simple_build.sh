#!/bin/bash
set -x
set -e
pushd /home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/Probes
make obj-intel64/decrypt_probe.so
popd
