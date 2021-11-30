#!/bin/bash

cut -d',' -f1,2,3 data0.csv > data0_only_site.csv 
cut -d',' -f1,2,3 data1.csv > data1_only_site.csv 
cut -d',' -f1,2,3 data2.csv > data2_only_site.csv 

cut -d',' -f1,2,3-13 data0.csv > data0_only_rdf.csv 
cut -d',' -f1,2,3-13 data1.csv > data1_only_rdf.csv 
cut -d',' -f1,2,3-13 data2.csv > data2_only_rdf.csv 

cut -d',' -f1,2,3,14-23 data0.csv > data0_only_value.csv 
cut -d',' -f1,2,3,14-23 data1.csv > data1_only_valuesite.csv 
cut -d',' -f1,2,3,14-23 data2.csv > data2_only_valuesite.csv 

  echo "only site"
  python /home/prithayan/project/pathPredictionProject/src/learnModel.py  -csv_file_name data1_only_site.csv -output_col 0 -test_csv_file_name data0_only_site.csv -save_plots data$i
  echo "only RDF"
  python /home/prithayan/project/pathPredictionProject/src/learnModel.py  -csv_file_name data1_only_rdf.csv -output_col 0 -test_csv_file_name data0_only_rdf.csv -save_plots data$i
  echo "only Value"
  python /home/prithayan/project/pathPredictionProject/src/learnModel.py  -csv_file_name data1_only_valuesite.csv -output_col 0 -test_csv_file_name data0_only_value.csv -save_plots data$i


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
