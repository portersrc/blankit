#!/bin/bash
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

echo > ldd.out
for folder in ${!bin_arr[@]}; do
    bin=${bin_arr[$folder]}
    echo $folder     >>  ldd.out
    ldd $folder/$bin >> ldd.out
    echo             >> ldd.out
done
