#!/bin/bash

folders=(
    401.bzip2
    403.gcc
    429.mcf
    433.milc
    444.namd
    445.gobmk
    462.libquantum
)

echo "benchmark  prediction-accuracy"
echo "---------  -------------------"
for folder in ${folders[@]}; do
    pushd $folder 1> /dev/null
    python ../learnModel.py \
      -csv_file_name train_data.csv \
      -test_csv_file_name test_data.csv \
      &> learnModel_output
    accuracy=`grep "Accuracy" learnModel_output | awk '{print $3}'`
    echo $folder  $accuracy
    popd 1> /dev/null
done
