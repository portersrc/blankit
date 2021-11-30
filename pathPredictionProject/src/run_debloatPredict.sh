#!/bin/bash
set -e
source ../../../../../shrc
rm -f debloat_log prediction_log learnModel_output learnModel_output 
export LD_LIBRARY_PATH=/home/prithayan/project/pathPredictionProject/src/blankit 
buildDIR=$PWD
echo "Compiling..."
#make clean ; make profile &> ${buildDIR}/debloat_log
#make clean
rm -f *.csv
rm -f ../../r*/r*/*.csv
cp -f  ../../../../Makefile.defaults.predict ../../../../Makefile.defaults
date
specmake &>${buildDIR}/debloat_log
date
bash ~/project/pathPredictionProject/src/get_runtest_cmd.sh  "$buildDIR"
echo "Done with generating commands"
rm -f train_data.csv
modes=( "test" "train" )
flag="1"
for mode in "${modes[@]}"
do
  if [ ! -d "../../run/run_base_${mode}_amd64-m64-gcc43-nn.0000/" ] ; then
    continue
  fi
  touch 1.csv;cp -f  *.csv ../../run/run_base_${mode}_amd64-m64-gcc43-nn.0000/
  pwd 
  cd ../../run/run_base_${mode}_amd64-m64-gcc43-nn.0000/
  pwd 
  threshold=0
  #let "threshold=0"
  echo "$threshold"
  echo "Training Running Pin..."
  while IFS='' read -r line || [[ -n "$line" ]]; do
    echo "$PWD:: running pin with $line"
    $PINTOOL -t $PIN/proccount.so -- $line  | $TOOL/parsePinTrace  &>> ${buildDIR}/debloat_log
    date
    if [ ! -f ${buildDIR}/train_data.csv ]; then
      #if echo $flag | grep --quiet -F "1"; then
      echo "first cat"
      cat training_calltrace.csv >>${buildDIR}/train_data.csv
      flag="0"
    else 
      echo "second cat"
      echo "">>${buildDIR}/train_data.csv
      tail -n +2 training_calltrace.csv >>${buildDIR}/train_data.csv
    fi
    (( threshold += 1 ))
    echo "counter:$threshold"
    if [ "$threshold" -gt "3" ] ; then 
      echo "DONE threshold, BREAK"
      break
    fi
  done < runme_train.sh
done
touch 1.csv;cp -f  *.csv $buildDIR
cd $buildDIR
#python $TOOL/learnModel.py -csv_file_name train_data.csv  -save_plots saved_dectree  &>${buildDIR}/learnModel_output
pwd
#python $TOOL/plot_unveil_tree_structure.py saved_dectree.pkl > decisionTree
#cp -f   ../../../../Makefile.defaults.embed ../../../../Makefile.defaults
#date
#specmake &>>${buildDIR}/debloat_log 
#date
#echo "Prediction Running Pin..."
#flag="1"
#rm -f test_data.csv
#touch 1.csv;cp -f  *.csv ../../run/run_base_ref_amd64-m64-gcc43-nn.0000/
#cd ../../run/run_base_ref_amd64-m64-gcc43-nn.0000/
##touch 1.csv;cp -f  *.csv ../../run/run_base_test_amd64-m64-gcc43-nn.0000/
##cd ../../run/run_base_test_amd64-m64-gcc43-nn.0000/
#echo "Training Running Pin..."
#threshold=0
#while IFS='' read -r line || [[ -n "$line" ]]; do
#  echo "$PWD:: running pin with $line"
#  $PINTOOL -t $PIN/proccount.so -- $line | $TOOL/parsePrediction  &>> ${buildDIR}/prediction_log 
#  date
#  (( threshold += 1 ))
#  echo "counter:$threshold"
#  if [ "$threshold" -gt "3" ] ; then 
#    echo "DONE threshold, BREAK"
#    break
#  fi
#done < runme_embedtest.sh
rm -f test_data.csv
touch 1.csv;cp -f  *.csv ../../run/run_base_ref_amd64-m64-gcc43-nn.0000/
threshold=0
cd ../../run/run_base_ref_amd64-m64-gcc43-nn.0000/
while IFS='' read -r line || [[ -n "$line" ]]; do
  echo "Test Running Pin..."
  echo "$PWD:: running pin with $line"
  $PINTOOL -t $PIN/proccount.so -- $line | $TOOL/parsePinTrace  &>> ${buildDIR}/debloat_log 
  date
  if [ ! -f ${buildDIR}/test_data.csv ]; then
    #if echo $flag | grep --quiet -F "1"; then
    cat training_calltrace.csv >>${buildDIR}/test_data.csv
    flag="0"
  else 
    echo "second cat"
    echo "">>${buildDIR}/test_data.csv
    tail -n +2 training_calltrace.csv >>${buildDIR}/test_data.csv
  fi
  (( threshold += 1 ))
  echo "counter:$threshold"
  if [ "$threshold" -gt "3" ] ; then 
    echo "DONE threshold, BREAK"
    break
  fi
done < runme_train.sh
touch 1.csv;cp -f  *.csv $buildDIR
cd $buildDIR
python $TOOL/learnModel.py -csv_file_name train_data.csv -test_csv_file_name test_data.csv -save_plots saved_dectree  &>>${buildDIR}/learnModel_output
