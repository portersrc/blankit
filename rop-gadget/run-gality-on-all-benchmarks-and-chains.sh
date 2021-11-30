#!/bin/bash

#
# 2019.02.20 cporter
#
# This script is placed at the root of the gality project and then run from
# there. On sloppyjoe, when it was used for usenix security 2019, that was
# here: /home/rudy/debloat/gality/gality.
#
# I preferred to keep it with the blankit repo code rather than in the gality
# project.
# 
# Obviously the hard-coded paths are nasty, etc. But this script could help
# if we ever need to run Gality across all of SPEC's benchmarks and call chains
# again.
#



# Example Gality invocation:
#java -cp ./bin/ gality.Program /home/rudy/debloat/pin_probe_debloat_experiments/rop-gadget/rop-gadgets-by-call-chain/403.gcc/403.gcc.0.gadget_instructions /home/rudy/debloat/pin_probe_debloat_experiments/rop-gadget/quality/403.gcc/403.gcc.0.gadget_quality




echo "Open this file and check comments before trying to run this again."
exit 1



#
# spec
# ----------
#
#root_input_folder=/home/rudy/debloat/pin_probe_debloat_experiments/rop-gadget/rop-gadgets-by-call-chain
#root_output_folder=/home/rudy/debloat/pin_probe_debloat_experiments/rop-gadget/quality
#input_folder=401.bzip2
#output_folder=401.bzip2
#for r in `ls $root_input_folder`; do
#    input_folder=$r
#    output_folder=$r
#    for f in `ls $root_input_folder/$input_folder`; do
#        java -cp ./bin/ gality.Program $root_input_folder/$input_folder/$f $root_output_folder/$output_folder/$f.quality
#    done
#done



#
# nginx
# ----------
#
##root_input_folder=/home/rudy/debloat/pin_probe_debloat_experiments/nginx/security-result/rop-gadget/rop-gadgets-by-call-chain
##root_output_folder=/home/rudy/debloat/pin_probe_debloat_experiments/nginx/security-result/rop-gadget/quality
#
##for f in `ls $root_input_folder`; do
##    java -cp ./bin/ gality.Program $root_input_folder/$f $root_output_folder/$f.quality
##done
#
#libs=(
#    libc-rop-gadgets.out
#    libdl-rop-gadgets.out
#    libpthread-rop-gadgets.out
#    libcrypt-rop-gadgets.out
#    libpcre-rop-gadgets.out
#    libz-rop-gadgets.out
#)
#
#for lib in ${libs[@]}; do
#    f_out=`basename $lib .out`
#    java -cp ./bin/ gality.Program /home/rudy/debloat/pin_probe_debloat_experiments/nginx/security-result/rop-gadget/lib-rop-gadgets/$lib /home/rudy/debloat/pin_probe_debloat_experiments/nginx/security-result/rop-gadget/lib-quality/$f_out.gadget_quality
#done






#
# sshd
# ----------
#
root_input_folder=/home/rudy/debloat/pin_probe_debloat_experiments/openssh/security/rop-gadget/rop-gadgets-by-call-chain
root_output_folder=/home/rudy/debloat/pin_probe_debloat_experiments/openssh/security/rop-gadget/quality
#
for f in `ls $root_input_folder`; do
    java -cp ./bin/ gality.Program $root_input_folder/$f $root_output_folder/$f.quality
done

#libs=(
#    libnsl-rop-gadgets.out
#    libresolv-rop-gadgets.out
#    libcrypt-rop-gadgets.out
#    libnss_compat-rop-gadgets.out
#    libnss_files-rop-gadgets.out
#    libnss_nis-rop-gadgets.out
#    libcrypto-rop-gadgets.out
#    libz-rop-gadgets.out
#    libutil-rop-gadgets.out
#    libdl-rop-gadgets.out
#    libssh-rop-gadgets.out
#    libc-rop-gadgets.out
#)
#
#for lib in ${libs[@]}; do
#    f_out=`basename $lib .out`
#    java -cp ./bin/ gality.Program /home/rudy/debloat/pin_probe_debloat_experiments/openssh/security/rop-gadget/lib-rop-gadgets/$lib /home/rudy/debloat/pin_probe_debloat_experiments/openssh/security/rop-gadget/lib-quality/$f_out.gadget_quality
#done
