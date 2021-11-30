#!/bin/bash
set -x

# hacky file, leaving for reference
#
# first block comment was to grab small-ish test and train data that we
# could run for artifact evaluation
# note: omnetpp ended up being too big (about 500MB), so i deleted it.
#
# second block comment to clean
#
# third section actually runs it.
# but see the driver for the proper way to do it.

: '
paths=(
  "./project/spec/t/benchspec/CPU2006/401.bzip2/build/build_base_amd64-m64-gcc43-nn.0000"
  "./project/spec/t/benchspec/CPU2006/444.namd/build/build_base_amd64-m64-gcc43-nn.0000"
  "./project/spec/t/benchspec/CPU2006/429.mcf/build/build_base_amd64-m64-gcc43-nn.0000"
  "./project/spec/t/benchspec/CPU2006/445.gobmk/build/build_base_amd64-m64-gcc43-nn.0000"
  "./project/spec/t/benchspec/CPU2006/403.gcc/build/build_base_amd64-m64-gcc43-nn.0000"
  "./project/spec/t/benchspec/CPU2006/471.omnetpp/build/build_base_amd64-m64-gcc43-nn.0000"
  "./project/spec/t/benchspec/CPU2006/433.milc/build/build_base_amd64-m64-gcc43-nn.0000"
  "./project/spec/t/benchspec/CPU2006/462.libquantum/build/build_base_amd64-m64-gcc43-nn.0000"
)


for path in ${paths[@]}; do
    mkdir -p $path
    scp ashwa:/home/prithayan/${path}/{test_data,train_data}.csv $path
done

'


: '
folders=(
    401.bzip2
    403.gcc
    429.mcf
    433.milc
    444.namd
    445.gobmk
    462.libquantum
    471.omnetpp
)
for f in ${folders[@]}; do
  mv $f/build/build_base_amd64-m64-gcc43-nn.0000/* $f
  rm -rf $f/build
done
'

folders=(
    401.bzip2
    403.gcc
    429.mcf
    433.milc
    444.namd
    445.gobmk
    462.libquantum
    471.omnetpp
)

for folder in ${folders[@]}; do
  pushd $folder
  python ../learnModel.py \
    -csv_file_name train_data.csv \
    -test_csv_file_name test_data.csv \
    &> learnModel_output
  cat learnModel_output
  popd
done
