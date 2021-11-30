#!/bin/bash

#
# MUST BE RUN AS ROOT
#

#$PINTOOL -t $PIN/proccount.so -- $line  | $TOOL/parsePinTrace  &>> ${buildDIR}/debloat_log

PIN_ROOT=/home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux
SSHD_ROOT=/home/rudy/debloat/pin_probe_debloat_experiments/openssh/openssh-bins/instpin_with_so
LIBSSH_ROOT=/home/rudy/debloat/pin_probe_debloat_experiments/openssh/openssh-bins/libs_so

SSH_HOST_KEY=/etc/ssh/ssh_host_rsa_key
SSHD_CONFIG=$SSHD_ROOT/sshd_config 

SSHD_BIN=$SSHD_ROOT/sshd_instrumentpin
SSHD_ARGS="-D -f $SSHD_CONFIG -h $SSH_HOST_KEY"
#SSHD_ARGS="-d -D -f $SSHD_CONFIG -h $SSH_HOST_KEY"




export LD_LIBRARY_PATH=$LIBSSH_ROOT:$LD_LIBRARY_PATH
#$SSHD_BIN $SSHD_ARGS

$PIN_ROOT/pin -t \
  $PIN_ROOT/source/tools/ManualExamples/obj-intel64/proccount.so \
  -- $SSHD_BIN $SSHD_ARGS
