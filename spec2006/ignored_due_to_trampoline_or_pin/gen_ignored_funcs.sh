#!/bin/bash
#for f in `ls *.txt`; do echo $f; grep "not adding probe" $f | wc -l; done

for f in `ls *.txt`; do
    grep "not adding probe" $f | awk '{print $3}' > $f.ignored_funcs
done
