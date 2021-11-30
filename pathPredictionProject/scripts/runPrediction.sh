#!/bin/bash

i=0
for var in "$@"
do
  echo "$i=$var"
  commands[i]=$var 
  i=$(( $i + 1 ))

done

i=0
for cmd in "${commands[@]}"
do
  echo $cmd
  rm gdbLog
  gdb --batch --command=~/project/pathPredictionProject/scripts/gdbDumpFCalls.script -args $cmd 
  /home/prithayan/project/pathPredictionProject/src/2levelBTparse gdbLog > parsedLog
  cat parsedLog | grep ^% | sed 's/%//' > data$i.csv
  python /home/prithayan/project/pathPredictionProject/src/learnModel.py  -csv_file_name data$i.csv -output_col 0 -test_csv_file_name data0.csv -save_plots data$i
  if [ "$i" -ne "0" ]; then
    tail -n +2 data$i.csv >> data0.csv
  fi
  i=$(( $i + 1 ))
done


##rm gdbLog
#cmd1=$1
#cmd2=$2
#echo $cmd1
#echo $cmd2
#gdb --batch --command=~/project/pathPredictionProject/scripts/gdbDumpFCalls.script -args $cmd1
#/home/prithayan/project/pathPredictionProject/src/2levelBTparse gdbLog > parsedLog
#cat parsedLog | grep ^% | sed 's/%//' > data1.csv
#python /home/prithayan/project/pathPredictionProject/src/learnModel.py  -csv_file_name data1.csv -output_col 0 -test_csv_file_name data1.csv
#
#rm gdbLog
#gdb --batch --command=~/project/pathPredictionProject/scripts/gdbDumpFCalls.script -args $cmd2 
#/home/prithayan/project/pathPredictionProject/src/2levelBTparse gdbLog > parsedLog
#cat parsedLog | grep ^% | sed 's/%//' > data2.csv
#python /home/prithayan/project/pathPredictionProject/src/learnModel.py  -csv_file_name data2.csv -output_col 0 -test_csv_file_name data2.csv
##
#python /home/prithayan/project/pathPredictionProject/src/learnModel.py  -csv_file_name data1.csv -output_col 0 -test_csv_file_name data2.csv -save_plots data1
#python /home/prithayan/project/pathPredictionProject/src/learnModel.py  -csv_file_name data2.csv -output_col 0 -test_csv_file_name data1.csv -save_plots data2
##
