#!/bin/bash
set -x
set -e

csv_arr=(
    ./401.bzip2/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./429.mcf/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./444.namd/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./450.soplex/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./453.povray/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./456.hmmer/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./471.omnetpp/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./473.astar/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./482.sphinx3/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
)

first_filtered_csv_arr=(
    ./401.bzip2/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./433.milc/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./450.soplex/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./453.povray/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./456.hmmer/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./458.sjeng/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./464.h264ref/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./470.lbm/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./471.omnetpp/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./473.astar/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./482.sphinx3/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./483.xalancbmk/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
)

: '
bin_arr=(
    ./401.bzip2/build/build_base_amd64-m64-gcc43-nn.0000/401.bzip2_embedpin
    ./429.mcf/build/build_base_amd64-m64-gcc43-nn.0000/429.mcf_embedpin
    ./444.namd/build/build_base_amd64-m64-gcc43-nn.0000/444.namd_embedpin
    ./450.soplex/build/build_base_amd64-m64-gcc43-nn.0000/450.soplex_embedpin
    ./453.povray/build/build_base_amd64-m64-gcc43-nn.0000/453.povray_embedpin
    ./456.hmmer/build/build_base_amd64-m64-gcc43-nn.0000/456.hmmer_embedpin
    ./471.omnetpp/build/build_base_amd64-m64-gcc43-nn.0000/471.omnetpp_embedpin
    ./473.astar/build/build_base_amd64-m64-gcc43-nn.0000/473.astar_embedpin
    ./482.sphinx3/build/build_base_amd64-m64-gcc43-nn.0000/482.sphinx3_embedpin
)
'

: '
bin_arr=(
    ./401.bzip2/build/build_base_amd64-m64-gcc43-nn.0000/401.bzip2_embedpin
    ./433.milc/build/build_base_amd64-m64-gcc43-nn.0000/433.milc_embedpin
    ./450.soplex/build/build_base_amd64-m64-gcc43-nn.0000/450.soplex_embedpin
    ./453.povray/build/build_base_amd64-m64-gcc43-nn.0000/453.povray_embedpin
    ./456.hmmer/build/build_base_amd64-m64-gcc43-nn.0000/456.hmmer_embedpin
    ./458.sjeng/build/build_base_amd64-m64-gcc43-nn.0000/458.sjeng_embedpin
    ./464.h264ref/build/build_base_amd64-m64-gcc43-nn.0000/464.h264ref_embedpin
    ./470.lbm/build/build_base_amd64-m64-gcc43-nn.0000/470.lbm_embedpin
    ./471.omnetpp/build/build_base_amd64-m64-gcc43-nn.0000/471.omnetpp_embedpin
    ./473.astar/build/build_base_amd64-m64-gcc43-nn.0000/473.astar_embedpin
    ./482.sphinx3/build/build_base_amd64-m64-gcc43-nn.0000/482.sphinx3_embedpin
    ./483.xalancbmk/build/build_base_amd64-m64-gcc43-nn.0000/483.xalancbmk_embedpin
)
'

bin_arr=(
    ./401.bzip2/build/build_base_amd64-m64-gcc43-nn.0000/401.bzip2_embedpin
    ./445.gobmk/build/build_base_amd64-m64-gcc43-nn.0000/445.gobmk_embedpin
    ./462.libquantum/build/build_base_amd64-m64-gcc43-nn.0000/462.libquantum_embedpin
    ./482.sphinx3/build/build_base_amd64-m64-gcc43-nn.0000/482.sphinx3_embedpin
    ./403.gcc/build/build_base_amd64-m64-gcc43-nn.0000/403.gcc_embedpin
    ./450.soplex/build/build_base_amd64-m64-gcc43-nn.0000/450.soplex_embedpin
    ./464.h264ref/build/build_base_amd64-m64-gcc43-nn.0000/464.h264ref_embedpin
    ./483.xalancbmk/build/build_base_amd64-m64-gcc43-nn.0000/483.xalancbmk_embedpin
    ./429.mcf/build/build_base_amd64-m64-gcc43-nn.0000/429.mcf_embedpin
    ./453.povray/build/build_base_amd64-m64-gcc43-nn.0000/453.povray_embedpin
    ./470.lbm/build/build_base_amd64-m64-gcc43-nn.0000/470.lbm_embedpin
    ./433.milc/build/build_base_amd64-m64-gcc43-nn.0000/433.milc_embedpin
    ./456.hmmer/build/build_base_amd64-m64-gcc43-nn.0000/456.hmmer_embedpin
    ./471.omnetpp/build/build_base_amd64-m64-gcc43-nn.0000/471.omnetpp_embedpin
    ./444.namd/build/build_base_amd64-m64-gcc43-nn.0000/444.namd_embedpin
    ./458.sjeng/build/build_base_amd64-m64-gcc43-nn.0000/458.sjeng_embedpin
    ./473.astar/build/build_base_amd64-m64-gcc43-nn.0000/473.astar_embedpin
)

second_filtered_csv_arr=(
    ./401.bzip2/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./403.gcc/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./429.mcf/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./433.milc/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./444.namd/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./445.gobmk/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./450.soplex/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./453.povray/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./456.hmmer/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./458.sjeng/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./462.libquantum/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./464.h264ref/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./470.lbm/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./471.omnetpp/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./473.astar/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./482.sphinx3/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
    ./483.xalancbmk/build/build_base_amd64-m64-gcc43-nn.0000/string2id_map.csv
)

benchmark_folders=(
	401.bzip2
	403.gcc
	#429.mcf
	#433.milc
	444.namd
	445.gobmk
	450.soplex
	453.povray
	456.hmmer
	#458.sjeng
	#462.libquantum
	464.h264ref
	#470.lbm
	471.omnetpp
	#473.astar
	482.sphinx3
	483.xalancbmk
)



#BASE_FOLDER=/home/prithayan/project/spec/spec2006/benchspec/CPU2006
#BASE_FOLDER=/home/prithayan/project/spec/mod_spec/benchspec/CPU2006
BASE_FOLDER=/home/prithayan/project/spec/testing_spec/benchspec/CPU2006
MACHINE=ashwa
#BASE_FOLDER=/nethome/girish/Developement/llvm3.8.0/Workloads/spec2006/StoreExecute/Wopin
#MACHINE=jedi026

function copy_csv() {
    #for csv in ${csv_arr[@]}; do
    for csv in ${second_filtered_csv_arr[@]}; do
        benchmark_folder=$(echo $csv | cut -d '/' -f 2)
        scp $MACHINE:$BASE_FOLDER/$csv $benchmark_folder
    done
}

function copy_embedpin() {
    for binary in ${bin_arr[@]}; do
        benchmark_folder=$(echo $binary | cut -d '/' -f 2)
        scp $MACHINE:$BASE_FOLDER/$binary $benchmark_folder
    done
}

function copy_run_test() {
    for benchmark_folder in ${benchmark_folders[@]}; do
        scp $MACHINE:$BASE_FOLDER/$benchmark_folder/run_test.sh $benchmark_folder
    done
}

#copy_embedpin
copy_csv
#copy_run_test
