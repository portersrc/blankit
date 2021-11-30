#!/bin/bash
set -x

if [ $# != 4 ]; then
    echo
    echo "Usage:"
    echo "    $0 <jit_output_filename> <this_csv> <train_csv_filename> <test_csv_filename>"
    echo
    echo "See script for more details"
    echo
    exit 1
fi

RUNME_ROOT=$HOME/debloat/pin_probe_debloat_experiments/nginx/prediction/runme

jit_file_ext=$1
jit_file=$(basename $jit_file_ext .out)
jit_file_reordered=${jit_file}.reordered.out
jit_file_reordered_nopid=${jit_file}.reordered.nopid.out

this_csv=$2
train_file=$3
test_file=$4



# Reorder the log
rm -f $jit_file_reordered
pids=$(cat ${jit_file_ext} | awk '{print $1}' | sort | uniq)
for pid in ${pids[@]}; do
    grep "^$pid" $jit_file_ext >  ${jit_file}.${pid}.out
    cat ${jit_file}.${pid}.out >> $jit_file_reordered
done
sed -E 's/^[0-9]+ //' $jit_file_reordered > $jit_file_reordered_nopid



# Get learning model and accuracy results
cat $jit_file_reordered_nopid | $RUNME_ROOT/parsePinTrace_libonly &> parse.log
cp training_calltrace.csv $this_csv
python2 $RUNME_ROOT/learnModel.py -csv_file_name $train_file -test_csv_file_name $test_file -save_plots saved_dectree >> learnModel_output
cat learnModel_output




# old code:
#cat test-getpid-worker.out | ./parsePinTrace_libonly &>> test-getpid-worker_debloat.log
#cp training_calltrace.csv train_data.csv
#python2 learnModel.py -csv_file_name train_data.csv -test_csv_file_name train_data.csv -save_plots saved_dectree >>learnModel_output
#cat learnModel_output

#cat test3s.out | ./parsePinTrace_libonly &>> test3s.log
#cp training_calltrace.csv test3s_data.csv
#python2 learnModel.py -csv_file_name train_data.csv -test_csv_file_name test3s_data.csv -save_plots saved_dectree >>learnModel_output
#cat learnModel_output

#cat train.out | ./parsePinTrace_libonly &>> train.log
#cp training_calltrace.csv train.csv
#python2 learnModel.py -csv_file_name train.csv -test_csv_file_name train.csv -save_plots saved_dectree >>learnModel_output
#cat learnModel_output


#cat getpid4s.out | ./parsePinTrace_libonly &>> getpid4s.log
#cp training_calltrace.csv test_data.csv
#python2 learnModel.py -csv_file_name test_data.csv -test_csv_file_name test_data.csv -save_plots saved_dectree >>learnModel_output
#cat learnModel_output
