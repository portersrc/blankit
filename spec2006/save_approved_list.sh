#!/bin/bash

set -e
#set -x


benchmarks=(
    401.bzip2
    403.gcc
    429.mcf
    433.milc
    444.namd
    445.gobmk
    450.soplex
    453.povray
    456.hmmer
    458.sjeng
    462.libquantum
    464.h264ref
    470.lbm
    471.omnetpp
    473.astar
    482.sphinx3
    483.xalancbmk
)

for b in ${benchmarks[@]}; do
    #cp Wopin/$b/approved_list_libm.txt Wopin_ref/$b/approved_list_libm.txt
    #cp Wopin/$b/string2id_map.csv Wopin_ref/$b/string2id_map.csv
    #cp Wopin/$b/string2id_map.csv Wopin_ref/$b/string2id_map.csv
    #cp Wopin_ref/$b/approved_list.txt.v4 v4-approved-lists/$b.approved_list.txt
    #cp Wopin_ref/$b/string2id_map.csv old-string2id_map-Wopin_ref/$b.string2id_map.csv

    set +e
    #cp Wopin_ref/$b/approved_list_libm.txt v4-approved-lists-libm/$b.approved_list_libm.txt
    #cp Wopin_ref/$b/approved_list_libgcc.txt v4-approved-lists-libgcc/$b.approved_list_libgcc.txt
    cp Wopin_ref/$b/approved_list_libstdcpp.txt v4-approved-lists-libstdcpp/$b.approved_list_libstdcpp.txt
done
