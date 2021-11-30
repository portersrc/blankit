#!/bin/bash

set -e
set -x
: '
#if [ $# != 1 ]; then
#    echo
#    echo "Usage:"
#    echo "  $0 benchmark"
#    echo
#    exit 1
#fi
#cp called_funcs_in_$1.txt called_funcs_in_$1.txt.bk
#cp approved_list.txt approved_list.txt.bk
#cp approved_list_of_called_funcs.txt approved_list_of_called_funcs.txt.bk
#cp ignored_list.txt ignored_list.txt.bk
'


: '
for csv in $(find -name *.csv);do
    #echo $csv
    cp $csv $csv.first_filter
done
'


declare -A bin_arr=(
    [401.bzip2]=401.bzip2_embedpin
    [403.gcc]=403.gcc_embedpin
    [429.mcf]=429.mcf_embedpin
    [433.milc]=433.milc_embedpin
    [444.namd]=444.namd_embedpin
    [445.gobmk]=445.gobmk_embedpin
    [450.soplex]=450.soplex_embedpin
    [453.povray]=453.povray_embedpin
    [456.hmmer]=456.hmmer_embedpin
    [458.sjeng]=458.sjeng_embedpin
    [462.libquantum]=462.libquantum_embedpin
    [464.h264ref]=464.h264ref_embedpin
    [470.lbm]=470.lbm_embedpin
    [471.omnetpp]=471.omnetpp_embedpin
    [473.astar]=473.astar_embedpin
    [482.sphinx3]=482.sphinx3_embedpin
    [483.xalancbmk]=483.xalancbmk_embedpin
)
declare -A called_funcs_arr=(
    [401.bzip2]=called_funcs_in_bzip.txt
    [403.gcc]=called_funcs_in_gcc.txt
    [429.mcf]=called_funcs_in_mcf.txt
    [433.milc]=called_funcs_in_milc.txt
    [444.namd]=called_funcs_in_namd.txt
    [445.gobmk]=called_funcs_in_gobmk.txt
    [450.soplex]=called_funcs_in_soplex.txt
    [453.povray]=called_funcs_in_povray.txt
    [456.hmmer]=called_funcs_in_hmmer.txt
    [458.sjeng]=called_funcs_in_sjeng.txt
    [462.libquantum]=called_funcs_in_libquantum.txt
    [464.h264ref]=called_funcs_in_h264ref.txt
    [470.lbm]=called_funcs_in_lbm.txt
    [471.omnetpp]=called_funcs_in_omnetpp.txt
    [473.astar]=called_funcs_in_astar.txt
    [482.sphinx3]=called_funcs_in_sphinx.txt
    [483.xalancbmk]=called_funcs_in_xalancbmk.txt
)


for bin_folder in ${!bin_arr[@]}; do
    #echo $bin_folder
    #echo ${bin_arr[$bin_folder]}
    cp $bin_folder/${bin_arr[$bin_folder]} $bin_folder/${bin_arr[$bin_folder]}.v3
done


: '
for called_funcs_folder in ${!called_funcs_arr[@]}; do
    #echo $called_funcs_folder
    #echo ${called_funcs_arr[$called_funcs_folder]}
    cp $called_funcs_folder/${called_funcs_arr[$called_funcs_folder]} $called_funcs_folder/${called_funcs_arr[$called_funcs_folder]}.v3
done
'
