#!/bin/bash


for f in `ls old-string2id_map-Wopin_ref`; do
    benchmark=$(echo $f | cut -d '.' -f 1,2)
    diff -q old-string2id_map-Wopin_ref/$f Wopin_ref/$benchmark/string2id_map.csv
    #diff -q old-string2id_map-Wopin_ref/$f Wopin/$benchmark/string2id_map.csv
done
