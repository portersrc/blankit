#!/bin/bash

#the_lib=$1
#the_lib=../example_app/pin/libpin.so
#the_lib=../libc/libc.so
#the_lib=/lib/x86_64-linux-gnu/libm.so.6
#the_lib=/lib/x86_64-linux-gnu/libgcc_s.so.1
#the_lib=/usr/lib/x86_64-linux-gnu/libstdc++.so.6


LIBROOT=/home/rudy/debloat/pin_probe_debloat_experiments/nginx/bin/libs

libs=(
    libcrypt.so
    libdl.so
    libpcre.so
    libpthread.so
    libz.so
)


#objdump -Fd $the_lib > out.offsets
#nm --print-size --size-sort --radix=d $the_lib > out.sizes

# column meanings:
# https://stackoverflow.com/questions/3065535/what-are-the-meanings-of-the-columns-of-the-symbol-table-displayed-by-readelf

#readelf -s $the_lib > out.all_columns
#readelf -s $the_lib | awk '{print $8" "$2" "$3}' > libc_name_offset_size.out

#readelf -s $the_lib | awk '{print $8" "$2" "$3}' > libm_name_offset_size.out

#readelf -s $the_lib | awk '{print $8" "$2" "$3}' > libgcc_name_offset_size.out


for lib in ${libs[@]}; do
    lib_base=`basename $lib .so`
    readelf -s $LIBROOT/$lib | awk '{print $8" "$2" "$3}' > ${lib_base}_name_offset_size.out
done
