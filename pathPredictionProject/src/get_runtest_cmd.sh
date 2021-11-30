#!/bin/bash
buildDIR=$1
modes=( "test" "train" "ref" )
rm -f ../../run/run_base_${mode}_amd64-m64-gcc43-nn.0000/runme_embedtest.sh
for mode in "${modes[@]}"
do
  if [ -d "../../run/run_base_${mode}_amd64-m64-gcc43-nn.0000" ]; then 
    rm -f  ../../run/run_base_${mode}_amd64-m64-gcc43-nn.0000/runme_train.sh
    rm  -f ../../run/run_base_${mode}_amd64-m64-gcc43-nn.0000/runme_embedtest.sh
    rm -f  ${buildDIR}/runme_train.sh
    rm  -f ${buildDIR}/runme_embedtest.sh
    runDIR="../../run/run_base_${mode}_amd64-m64-gcc43-nn.0000"
    specCmdsFile="../../run/run_base_${mode}_amd64-m64-gcc43-nn.0000/speccmds.cmd"
    cd ../../;result=${PWD##*/};cd -
    exeName=$result
    filecontents=`cat  ../../run/run_base_${mode}_amd64-m64-gcc43-nn.0000/speccmds.cmd`
    echo "working with $mode"
    while IFS='' read -r line || [[ -n "$line" ]]; do
      filecontents="$line"
      flag="NO"
      executable1="../../build/build_base_amd64-m64-gcc43-nn.0000/${exeName}_instrumentpin "
      executable2="../../build/build_base_amd64-m64-gcc43-nn.0000/${exeName}_embedpin "
      runCMD=" "
      for word in $filecontents
      do
        #echo "Got word::$word"
        if echo $flag | grep --quiet -F "YES"; then
          #echo "copying $word"
          #cp -f ../../run/run_base_train_amd64-m64-gcc43-nn.0000/$word ./
          runCMD="$runCMD $word"
        fi
        if echo $word | grep --quiet -F "../run_base_${mode}_amd64-m64-gcc43-nn.0000"; then
          #echo "$word::exists"
          flag="YES"
        fi

      done
      #echo "run cmd::$runCMD"
      pwd
      echo "$executable1 $runCMD" >> ${runDIR}/runme_train.sh
      echo "$executable2 $runCMD" >> ${runDIR}/runme_embedtest.sh
    done < "$specCmdsFile"
  fi
done
