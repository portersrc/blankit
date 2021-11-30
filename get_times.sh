#!/bin/bash
results_file=$1
times_file=$results_file.times
grep "pin main time" $results_file  | awk '{print $5}' >  $times_file
grep "load time .*libc.so" $results_file  | awk '{print $6}' >>  $times_file
grep "load time .*libm.so" $results_file  | awk '{print $6}' >>  $times_file
grep "load time .*libstdc++.so" $results_file  | awk '{print $6}' >>  $times_file
grep "load time .*libgcc_s.so" $results_file  | awk '{print $6}' >>  $times_file
grep -P "real\t" $results_file | awk '{print $2}'       >> $times_file
#sed -i 's/0m//g' $times_file
#sed -i 's/s//g' $times_file
#cat $times_file
./parse_time.py $times_file
