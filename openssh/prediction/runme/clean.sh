#!/bin/bash

if [ "$1" != "" ]; then
    mv $1 ..
fi

rm -f *.out
rm -f string2id_map.csv  saved_dectree.*
rm -f training_calltrace.csv 
rm -f *.csv
rm -f learnModel_output parse.log

if [ "$1" != "" ]; then
    mv ../$1 .
fi
