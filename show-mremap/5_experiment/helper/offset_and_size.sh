#!/bin/bash

#the_lib=$1
#the_lib=../example_app/pin/libpin.so
#the_lib=../libc/libc.so
#the_lib=/lib/x86_64-linux-gnu/libm.so.6
#the_lib=/lib/x86_64-linux-gnu/libgcc_s.so.1
#the_lib=/usr/lib/x86_64-linux-gnu/libstdc++.so.6

#objdump -Fd $the_lib > out.offsets
#nm --print-size --size-sort --radix=d $the_lib > out.sizes

# column meanings:
# https://stackoverflow.com/questions/3065535/what-are-the-meanings-of-the-columns-of-the-symbol-table-displayed-by-readelf

#readelf -s $the_lib > out.all_columns
#readelf -s $the_lib | awk '{print $8" "$2" "$3}' > libc_name_offset_size.out

#readelf -s $the_lib | awk '{print $8" "$2" "$3}' > libm_name_offset_size.out

#readelf -s $the_lib | awk '{print $8" "$2" "$3}' > libgcc_name_offset_size.out

readelf -s libfoo/libfoo.so | awk '{print $8" "$2" "$3}' > name-offset-size-libfoo.txt
