#!/bin/bash
filecontents=`cat ../../run/run_base_test_amd64-m64-gcc43-nn.0000/speccmds.cmd`
flag="NO"
cd ../../;result=${PWD##*/};cd -
exeName=$result
runCMD="${exeName}_instrumentpin "
for word in $filecontents
do
  if echo $flag | grep --quiet -F "YES"; then
    echo "$word::command"
    cp ../../run/run_base_test_amd64-m64-gcc43-nn.0000/$word ./
    runCMD="$runCMD $word"
  fi
  if echo $word | grep --quiet -F "../run_base_test_amd64-m64-gcc43-nn.0000"; then
    echo "$word::exists"
    flag="YES"
  fi

done
echo "final command::$runCMD"
echo "./$runCMD" > ./runme_train.sh
filecontents=`cat ../../run/run_base_train_amd64-m64-gcc43-nn.0000/speccmds.cmd`
flag="NO"
cd ../../;result=${PWD##*/};cd -
exeName=$result
runCMD="${exeName}_instrumentpin "
for word in $filecontents
do
  if echo $flag | grep --quiet -F "YES"; then
    echo "$word::command"
    cp ../../run/run_base_train_amd64-m64-gcc43-nn.0000/$word ./
    runCMD="$runCMD $word"
  fi
  if echo $word | grep --quiet -F "../run_base_train_amd64-m64-gcc43-nn.0000"; then
    echo "$word::exists"
    flag="YES"
  fi

done
echo "final command::$runCMD"
echo "./$runCMD" > ./runme_test.sh
