#!/bin/bash
#/home/rudy/debloat/ROPgadget/ROPgadget.py --binary /home/rudy/debloat/pin_probe_debloat_experiments/openssh/libs/libcrypt.so &> libcrypt-rop-gadgets.out
#/home/rudy/debloat/ROPgadget/ROPgadget.py --binary /home/rudy/debloat/pin_probe_debloat_experiments/openssh/libs/libcrypto.so &> libcrypto-rop-gadgets.out
#/home/rudy/debloat/ROPgadget/ROPgadget.py --binary /home/rudy/debloat/pin_probe_debloat_experiments/openssh/libs/libdl.so &> libdl-rop-gadgets.out
#/home/rudy/debloat/ROPgadget/ROPgadget.py --binary /home/rudy/debloat/pin_probe_debloat_experiments/openssh/libs/libnsl.so &> libnsl-rop-gadgets.out
#/home/rudy/debloat/ROPgadget/ROPgadget.py --binary /home/rudy/debloat/pin_probe_debloat_experiments/openssh/libs/libnss_compat.so &> libnss_compat-rop-gadgets.out
#/home/rudy/debloat/ROPgadget/ROPgadget.py --binary /home/rudy/debloat/pin_probe_debloat_experiments/openssh/libs/libnss_files.so &> libnss_files-rop-gadgets.out
#/home/rudy/debloat/ROPgadget/ROPgadget.py --binary /home/rudy/debloat/pin_probe_debloat_experiments/openssh/libs/libnss_nis.so &> libnss_nis-rop-gadgets.out
#/home/rudy/debloat/ROPgadget/ROPgadget.py --binary /home/rudy/debloat/pin_probe_debloat_experiments/openssh/libs/libresolv.so &> libresolv-rop-gadgets.out
#/home/rudy/debloat/ROPgadget/ROPgadget.py --binary /home/rudy/debloat/pin_probe_debloat_experiments/openssh/libs/libutil.so &> libutil-rop-gadgets.out
#/home/rudy/debloat/ROPgadget/ROPgadget.py --binary /home/rudy/debloat/pin_probe_debloat_experiments/openssh/libs/libz.so &> libz-rop-gadgets.out
/home/rudy/debloat/ROPgadget/ROPgadget.py --binary /home/rudy/debloat/pin_probe_debloat_experiments/openssh/openssh-bins/libs_so/libssh.so &> libssh-rop-gadgets.out
