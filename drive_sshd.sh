#!/bin/bash

#
# MUST BE RUN AS ROOT
#



PIN_ROOT=$HOME/else/pin-3.6-97554-g31f0a167d-gcc-linux
OPENSSH_ROOT=$HOME/debloat/pin_probe_debloat_experiments/openssh
OPENSSH_BIN_ROOT=$OPENSSH_ROOT/openssh-bins/orig_with_so
LIBSSH_ROOT=$OPENSSH_ROOT/openssh-bins/libs_so
LIST_ROOT=$OPENSSH_ROOT/approved_lists

SSH_HOST_KEY=/etc/ssh/ssh_host_rsa_key
SSHD_CONFIG=$OPENSSH_BIN_ROOT/sshd_config 

SSHD_BIN=$OPENSSH_BIN_ROOT/sshd
SSHD_BIN_AUDIT=$OPENSSH_BIN_ROOT/sshd_audit
SSHD_ARGS="-D -f $SSHD_CONFIG -h $SSH_HOST_KEY"
#SSHD_ARGS="-d -D -f $SSHD_CONFIG -h $SSH_HOST_KEY"


export BLANKIT_APP_NAME=$SSHD_BIN
export BLANKIT_APPROVED_LIST=$LIST_ROOT/approved_list.txt
export BLANKIT_APPROVED_LIST_LIBCRYPTO=$LIST_ROOT/approved_list_libcrypto.txt
export BLANKIT_APPROVED_LIST_LIBCRYPT=$LIST_ROOT/approved_list_libcrypt.txt
export BLANKIT_APPROVED_LIST_LIBDL=$LIST_ROOT/approved_list_libdl.txt
export BLANKIT_APPROVED_LIST_LIBRESOLV=$LIST_ROOT/approved_list_libresolv.txt
export BLANKIT_APPROVED_LIST_LIBSSH=$LIST_ROOT/approved_list_libssh.txt
export BLANKIT_APPROVED_LIST_LIBUTIL=$LIST_ROOT/approved_list_libutil.txt
export BLANKIT_APPROVED_LIST_LIBZ=$LIST_ROOT/approved_list_libz.txt
export BLANKIT_APPROVED_LIST_LIBNSS_COMPAT=$LIST_ROOT/approved_list_libnss_compat.txt
export BLANKIT_APPROVED_LIST_LIBNSL=$LIST_ROOT/approved_list_libnsl.txt
export BLANKIT_APPROVED_LIST_LIBNSS_NIS=$LIST_ROOT/approved_list_libnss_nis.txt
export BLANKIT_APPROVED_LIST_LIBNSS_FILES=$LIST_ROOT/approved_list_libnss_files.txt


export LD_LIBRARY_PATH=$LIBSSH_ROOT:$LD_LIBRARY_PATH
#$SSHD_BIN $SSHD_ARGS


if [ "$1" == "nopin" ]; then

    LD_LIBRARY_PATH=$HOME/debloat/pin_probe_debloat_experiments/openssh/openssh-bins/libs_so $HOME/debloat/pin_probe_debloat_experiments/openssh/openssh-bins/orig_with_so/sshd -D -f $HOME/debloat/pin_probe_debloat_experiments/openssh/openssh-bins/orig_with_so/sshd_config -h /etc/ssh/ssh_host_rsa_key
    #LD_LIBRARY_PATH=$HOME/debloat/pin_probe_debloat_experiments/openssh/openssh-bins/libs_so $HOME/debloat/pin_probe_debloat_experiments/openssh/openssh-bins/orig_with_so/sshd $2

elif [ "$1" == "audit" ]; then

    # XXX
    # XXX Requires the sshd-stderr version of the audit library here:
    # XXX $HOME/debloat/pathPredictionProject/src
    # XXX
    # XXX
    #LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/debloat/pathPredictionProject/src:$HOME/debloat/pin_probe_debloat_experiments/openssh/openssh-bins/libs_so \
    #               $SSHD_BIN_AUDIT $SSHD_ARGS
    LD_LIBRARY_PATH=$HOME/debloat/pin_probe_debloat_experiments/openssh/openssh-bins/libs_so:$HOME/debloat/pathPredictionProject/src $HOME/debloat/pin_probe_debloat_experiments/openssh/openssh-bins/orig_with_so/sshd_audit -D -f $HOME/debloat/pin_probe_debloat_experiments/openssh/openssh-bins/orig_with_so/sshd_config -h /etc/ssh/ssh_host_rsa_key

else
    $PIN_ROOT/pin -t \
      $PIN_ROOT/source/tools/Probes/obj-intel64/decrypt_probe.so \
      -- $SSHD_BIN $SSHD_ARGS
fi


