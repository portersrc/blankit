#!/bin/bash

#
# MUST BE RUN AS ROOT
#

LD_LIBRARY_PATH=/home/rudy/debloat/pin_probe_debloat_experiments/openssh/openssh-bins/libs_so /home/rudy/debloat/pin_probe_debloat_experiments/openssh/openssh-bins/orig_with_so/sshd -D -f /home/rudy/debloat/pin_probe_debloat_experiments/openssh/openssh-bins/orig_with_so/sshd_config -h /etc/ssh/ssh_host_rsa_key
