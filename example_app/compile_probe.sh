#!/bin/bash
set -x
set -e

#pushd /home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/Probes
#make obj-intel64/decrypt_probe.so
#popd


pushd /home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/Probes

mkdir -p obj-intel64/
g++ -Wall -Werror -Wno-unknown-pragmas -D__PIN__=1 -DPIN_CRT=1 -fno-stack-protector -fno-exceptions -funwind-tables -fasynchronous-unwind-tables -fno-rtti -DTARGET_IA32E -DHOST_IA32E -fPIC -DTARGET_LINUX -fabi-version=2  -I../../../source/include/pin -I../../../source/include/pin/gen -isystem /home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux/extras/stlport/include -isystem /home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux/extras/libstdc++/include -isystem /home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux/extras/crt/include -isystem /home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux/extras/crt/include/arch-x86_64 -isystem /home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux/extras/crt/include/kernel/uapi -isystem /home/rudy/else/pin-3.6-97554-g31f0a167d-gcc-linux/extras/crt/include/kernel/uapi/asm-x86 -I../../../extras/components/include -I../../../extras/xed-intel64/include/xed -I../../../source/tools/InstLib -O3 -fomit-frame-pointer -fno-strict-aliasing   -c -o obj-intel64/decrypt_probe.o decrypt_probe.cpp
#g++ -shared -Wl,--hash-style=sysv ../../../intel64/runtime/pincrt/crtbeginS.o -Wl,-Bsymbolic -Wl,--version-script=../../../source/include/pin/pintool.ver -fabi-version=2    -o obj-intel64/decrypt_probe.so obj-intel64/decrypt_probe.o  -L../../../intel64/runtime/pincrt -L../../../intel64/lib -L../../../intel64/lib-ext -L../../../extras/xed-intel64/lib -lpin -lxed ../../../intel64/runtime/pincrt/crtendS.o -lpin3dwarf  -ldl-dynamic -nostdlib -lstlport-dynamic -lm-dynamic -lc-dynamic -lunwind-dynamic
g++ -shared -Wl,--hash-style=sysv ../../../intel64/runtime/pincrt/crtbeginS.o -Wl,-Bsymbolic -Wl,--version-script=../../../source/include/pin/pintool.ver -fabi-version=2    -o obj-intel64/decrypt_probe.so obj-intel64/decrypt_probe.o  -L../../../intel64/runtime/pincrt -L../../../intel64/lib -L../../../intel64/lib-ext -L../../../extras/xed-intel64/lib -lrt -lpin -lxed ../../../intel64/runtime/pincrt/crtendS.o -lpin3dwarf  -ldl-dynamic -nostdlib -lstlport-dynamic -lm-dynamic -lc-dynamic -lunwind-dynamic

popd
