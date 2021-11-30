#!/bin/bash
#set -x
#set -e


PIN_ROOT=$HOME/else/pin-3.6-97554-g31f0a167d-gcc-linux
BENCHMARKS_ROOT=$HOME/debloat/pin_probe_debloat_experiments
LD_BLANKIT_ROOT=$HOME/debloat/pin_probe_debloat_experiments/blankit
LD_AUDIT_RUNTIME_ROOT=$HOME/balloons/llvm5/lib/Transforms/debloat/audit/audit_runtime/obj

declare -A benchmark_folders_mibench=(
    # mibench
    # -------
    #[basicmath]=basicmath
    [basicmath]=mibench/automotive/basicmath
    [susan]=susan/pri-sus
    [bitcount]=mibench/automotive/bitcount
    [qsort]=mibench/automotive/qsort # fails with full approved_list.txt
    [jpeg]=mibench/consumer/jpeg # fails with full approved_list.txt
    # lame fails to build
    # mad fails to build
    # what's up with tiff?
    [typeset]=mibench/consumer/typeset # fails with full
    [dijkstra]=mibench/network/dijkstra # fails with full
    [patricia]=mibench/network/patricia # has no calls that work
    # ghostscript fails to build
    # ispell fails to build
    # rsynth fails to configure
    # sphinx fails to configure
    [stringsearch]=mibench/office/stringsearch
    [blowfish]=mibench/security/blowfish # has no calls that work
    # pgp fails to compile
    # rijndael fails to compile
    [sha]=mibench/security/sha # fails on full
    [adpcm]=mibench/telecomm/adpcm # seems to break on trace? didn't complete
    [crc]=mibench/telecomm/CRC32 # fails on full
    [gsm]=mibench/telecomm/gsm # fails on full
)
declare -A benchmark_folders_test=(
    [bzip]=spec2006/Wopin/401.bzip2
    [gcc]=spec2006/Wopin/403.gcc
    [namd]=spec2006/Wopin/444.namd
    [gobmk]=spec2006/Wopin/445.gobmk
    [soplex]=spec2006/Wopin/450.soplex
    [povray]=spec2006/Wopin/453.povray
    [hmmer]=spec2006/Wopin/456.hmmer
    [h264ref]=spec2006/Wopin/464.h264ref
    [omnetpp]=spec2006/Wopin/471.omnetpp
    [sphinx]=spec2006/Wopin/482.sphinx3
    [xalancbmk]=spec2006/Wopin/483.xalancbmk
)
declare -A benchmark_folders_train=(
    [bzip]=spec2006/Wopin/401.bzip2
    [gcc]=spec2006/Wopin/403.gcc
    [mcf]=spec2006/Wopin/429.mcf
    [milc]=spec2006/Wopin/433.milc
    [namd]=spec2006/Wopin/444.namd
    [gobmk]=spec2006/Wopin/445.gobmk
    [soplex]=spec2006/Wopin/450.soplex
    [povray]=spec2006/Wopin/453.povray
    [hmmer]=spec2006/Wopin/456.hmmer
    [sjeng]=spec2006/Wopin/458.sjeng
    [libquantum]=spec2006/Wopin/462.libquantum
    [h264ref]=spec2006/Wopin/464.h264ref
    [lbm]=spec2006/Wopin/470.lbm
    [omnetpp]=spec2006/Wopin/471.omnetpp
    [astar]=spec2006/Wopin/473.astar
    [sphinx]=spec2006/Wopin/482.sphinx3
    [xalancbmk]=spec2006/Wopin/483.xalancbmk
)
declare -A benchmark_folders_ref=(
    [bzip]=spec2006/Wopin_ref/401.bzip2
    [gcc]=spec2006/Wopin_ref/403.gcc
    [mcf]=spec2006/Wopin_ref/429.mcf
    [milc]=spec2006/Wopin_ref/433.milc
    [namd]=spec2006/Wopin_ref/444.namd
    [gobmk]=spec2006/Wopin_ref/445.gobmk
    [soplex]=spec2006/Wopin_ref/450.soplex
    [povray]=spec2006/Wopin_ref/453.povray
    [hmmer]=spec2006/Wopin_ref/456.hmmer
    [sjeng]=spec2006/Wopin_ref/458.sjeng
    [libquantum]=spec2006/Wopin_ref/462.libquantum
    [h264ref]=spec2006/Wopin_ref/464.h264ref
    [lbm]=spec2006/Wopin_ref/470.lbm
    [omnetpp]=spec2006/Wopin_ref/471.omnetpp
    [astar]=spec2006/Wopin_ref/473.astar
    [sphinx]=spec2006/Wopin_ref/482.sphinx3
    [xalancbmk]=spec2006/Wopin_ref/483.xalancbmk
)

declare -A benchmarks_mibench=(
    [basicmath]=basicmath_small
    [susan]=susan
    [bitcount]=bitcnts
    [qsort]=qsort_small
    [jpeg]=jpeg-6a/cjpeg
    [typeset]=lout-3.24/lout
    [dijkstra]=dijkstra_small
    [patricia]=patricia
    [stringsearch]=search_small
    [blowfish]=bf
    [sha]=sha
    [adpcm]=bin/rawcaudio
    [crc]=crc
    [gsm]=bin/toast
)
declare -A benchmarks_raw=(
    [bzip]=401.bzip2_wopin
    [gcc]=403.gcc_wopin
    [mcf]=429.mcf_wopin
    [milc]=433.milc_wopin
    [namd]=444.namd_wopin
    [gobmk]=445.gobmk_wopin
    [soplex]=450.soplex_wopin
    [povray]=453.povray_wopin
    [hmmer]=456.hmmer_wopin
    [sjeng]=458.sjeng_wopin
    [libquantum]=462.libquantum_wopin
    [h264ref]=464.h264ref_wopin
    [lbm]=470.lbm_wopin
    [omnetpp]=471.omnetpp_wopin
    [astar]=473.astar_wopin
    [sphinx]=482.sphinx3_wopin
    [xalancbmk]=483.xalancbmk_wopin
)
declare -A benchmarks_embedpin=(
    [bzip]=401.bzip2_embedpin
    [gcc]=403.gcc_embedpin
    [mcf]=429.mcf_embedpin
    [milc]=433.milc_embedpin
    [namd]=444.namd_embedpin
    [gobmk]=445.gobmk_embedpin
    [soplex]=450.soplex_embedpin
    [povray]=453.povray_embedpin
    [hmmer]=456.hmmer_embedpin
    [sjeng]=458.sjeng_embedpin
    [libquantum]=462.libquantum_embedpin
    [h264ref]=464.h264ref_embedpin
    [lbm]=470.lbm_embedpin
    [omnetpp]=471.omnetpp_embedpin
    [astar]=473.astar_embedpin
    [sphinx]=482.sphinx3_embedpin
    [xalancbmk]=483.xalancbmk_embedpin
)
declare -A benchmarks_audit=(
    [bzip]=401.bzip2_audit
    [gcc]=403.gcc_audit
    [mcf]=429.mcf_audit
    [milc]=433.milc_audit
    [namd]=444.namd_audit
    [gobmk]=445.gobmk_audit
    [soplex]=450.soplex_audit
    [povray]=453.povray_audit
    [hmmer]=456.hmmer_audit
    [sjeng]=458.sjeng_audit
    [libquantum]=462.libquantum_audit
    [h264ref]=464.h264ref_audit
    [lbm]=470.lbm_audit
    [omnetpp]=471.omnetpp_audit
    [astar]=473.astar_audit
    [sphinx]=482.sphinx3_audit
    [xalancbmk]=483.xalancbmk_audit
)

declare -A benchmark_args_mibench=(
    [basicmath]=""
    [susan]="input_small.pgm output_small.smoothing.pgm -s"
    [bitcount]="75000"
    [qsort]="input_small.dat"
    [jpeg]="-dct int -progressive -opt -outfile output_small_encode.jpeg input_small.ppm"
    [typeset]="-I lout-3.24/include -D lout-3.24/data -F lout-3.24/font -C lout-3.24/maps -H lout-3.24/hyph small.lout"
    [dijkstra]="input.dat"
    [patricia]="small.udp"
    [stringsearch]=""
    [blowfish]="e input_small.asc output_small.enc 1234567890abcdeffedcba0987654321"
    [sha]="input_small.asc"
    [adpcm]="< data/small.pcm"
    [crc]="../adpcm/data/small.pcm"
    [gsm]="-fps -c data/small.au"
)
declare -A benchmark_args_test=(
    [bzip]="input.program 5"
    [gcc]="cccp.i -o cccp.s"
    [namd]="--input namd.input --iterations 1"
    [gobmk]="--quiet --mode gtp" # has a special case below for redirect
    [soplex]="-m10000 test.mps"
    [povray]="SPEC-benchmark-test.ini"
    [hmmer]="--fixed 0 --mean 325 --num 45000 --sd 200 --seed 0 bombesin.hmm"
    [h264ref]="-d foreman_test_encoder_baseline.cfg"
    [omnetpp]="omnetpp.ini_test"
    [sphinx]="ctlfile_test . args.an4_test"
    [xalancbmk]="-v test.xml xalanc.xsl_test"
)
declare -A benchmark_args_train=(
    [bzip]="input.program 10"
    [gcc]="integrate.i -o integrate.s"
    [mcf]="inp.in"
    [milc]="su3imp.in" # has a special case below for redirect
    [namd]="--input namd.input --iterations 1"
    [gobmk]="--quiet --mode gtp" # has a special case below for redirect
    [soplex]="-s1 -e -m5000 pds-20.mps"
    [povray]="SPEC-benchmark-train.ini"
    [hmmer]="--fixed 0 --mean 425 --num 85000 --sd 300 --seed 0 leng100.hmm"
    [sjeng]="test.txt"
    [libquantum]="143 25"
    [h264ref]="-d foreman_train_encoder_baseline.cfg"
    [lbm]="300 reference.dat 0 1 100_100_130_cf_b.of"
    [omnetpp]="omnetpp.ini"
    [astar]="rivers1.cfg"
    [sphinx]="ctlfile . args.an4"
    [xalancbmk]="-v allbooks.xml xalanc.xsl"
)
declare -A benchmark_args_ref=(
    [bzip]="input.source 280"
    [gcc]="166.i -o 166.s"
    [mcf]="inp.in"
    [milc]="su3imp.in" # has a special case below for redirect
    [namd]="--input namd.input --iterations 38"
    [gobmk]="--quiet --mode gtp" # has a special case below for redirect
    [soplex]="-s1 -e -m45000 pds-50.mps"
    [povray]="SPEC-benchmark-ref.ini"
    [hmmer]="--fixed 0 --mean 500 --num 500000 --sd 350 --seed 0 retro.hmm"
    [sjeng]="ref.txt"
    [libquantum]="1397 8"
    [h264ref]="-d foreman_ref_encoder_baseline.cfg"
    [lbm]="3000 reference.dat 0 0 100_100_130_ldc.of"
    [omnetpp]="omnetpp.ini"
    [astar]="BigLakes2048.cfg"
    [sphinx]="ctlfile . args.an4"
    [xalancbmk]="-v t5.xml xalanc.xsl"
)


function build() {
    pushd blankit
    make
    popd
    pushd $PIN_ROOT/source/tools/Probes
    make obj-intel64/decrypt_probe.so
    popd
}

function run() {
    echo "run()"
    BENCHMARK_FOLDER=${benchmark_folders[$1]}
    BENCHMARK=${benchmarks[$1]}
    BENCHMARK_ARGS=${benchmark_args[$1]}
    echo "benchmarks root:  $BENCHMARKS_ROOT"
    echo "benchmark folder: $BENCHMARK_FOLDER"
    echo "benchmark:        $BENCHMARK"
    echo "benchmark args:   $BENCHMARK_ARGS"

    export LD_LIBRARY_PATH=$LD_BLANKIT_ROOT:$LD_AUDIT_RUNTIME_ROOT
    export BLANKIT_APP_NAME=$BENCHMARKS_ROOT/$BENCHMARK_FOLDER/$BENCHMARK
    export BLANKIT_APPROVED_LIST=$BENCHMARKS_ROOT/$BENCHMARK_FOLDER/approved_list.txt
    export BLANKIT_APPROVED_LIST_LIBM=$BENCHMARKS_ROOT/$BENCHMARK_FOLDER/approved_list_libm.txt
    export BLANKIT_APPROVED_LIST_LIBGCC=$BENCHMARKS_ROOT/$BENCHMARK_FOLDER/approved_list_libgcc.txt
    export BLANKIT_APPROVED_LIST_LIBSTDCPP=$BENCHMARKS_ROOT/$BENCHMARK_FOLDER/approved_list_libstdcpp.txt
    export BLANKIT_PREDICT_SETS=$BENCHMARKS_ROOT/$BENCHMARK_FOLDER/string2id_map.csv

    pushd $BENCHMARKS_ROOT/$BENCHMARK_FOLDER
    # A couple benchmarks have funky redirect parameters that we account
    # for here (albeit in a nasty way)
    if [ $1 == "milc" ]; then
        if [ $3 == "raw" ] || [ $3 == "audit" ]; then
            time ./$BENCHMARK < $BENCHMARK_ARGS
        elif [ $3 == "auditmem" ]; then
            time valgrind ./$BENCHMARK < $BENCHMARK_ARGS
        else
            time $PIN_ROOT/pin -t \
              $PIN_ROOT/source/tools/Probes/obj-intel64/decrypt_probe.so \
              -- ./$BENCHMARK < $BENCHMARK_ARGS
        fi
        retval=$? # XXX other scripts that use run() depend on this
    elif [ $1 == "gobmk" ]; then
        if [ $3 == "raw" ] || [ $3 == "audit" ] || [ $3 == "auditmem" ] ; then
            # FIXME clean up this manure
            CMD_PREFIX='time'
            if [ $3 == "auditmem" ]; then
                CMD_PREFIX='time valgrind'
            fi
            if [ $2 == "test" ]; then
                $CMD_PREFIX ./$BENCHMARK $BENCHMARK_ARGS < capture.tst
            elif [ $2 == "train" ]; then
                $CMD_PREFIX ./$BENCHMARK $BENCHMARK_ARGS < arb.tst
            else # ref
                $CMD_PREFIX ./$BENCHMARK $BENCHMARK_ARGS < 13x13.tst
            fi
        else
            if [ $2 == "test" ]; then
                time $PIN_ROOT/pin -t \
                  $PIN_ROOT/source/tools/Probes/obj-intel64/decrypt_probe.so \
                  -- ./$BENCHMARK $BENCHMARK_ARGS < capture.tst
            elif [ $2 == "train" ]; then
                time $PIN_ROOT/pin -t \
                  $PIN_ROOT/source/tools/Probes/obj-intel64/decrypt_probe.so \
                  -- ./$BENCHMARK $BENCHMARK_ARGS < arb.tst
            else # ref
                time $PIN_ROOT/pin -t \
                  $PIN_ROOT/source/tools/Probes/obj-intel64/decrypt_probe.so \
                  -- ./$BENCHMARK $BENCHMARK_ARGS < 13x13.tst
            fi
        fi
        retval=$? # XXX other scripts that use run() depend on this
    else
        if [ $3 == "raw" ] || [ $3 == "audit" ]; then
            time ./$BENCHMARK $BENCHMARK_ARGS
        elif [ $3 == "auditmem" ]; then
            time valgrind ./$BENCHMARK $BENCHMARK_ARGS
        else
            time $PIN_ROOT/pin -t \
              $PIN_ROOT/source/tools/Probes/obj-intel64/decrypt_probe.so \
              -- ./$BENCHMARK $BENCHMARK_ARGS
        fi
        retval=$? # XXX other scripts that use run() depend on this
    fi
    popd
}

function usage() {
    echo
    echo "Usage:"
    echo "  $0 build"
    echo "  $0 trace [bzip, gcc, ...] [test, train, ref]"
    echo "  $0 run   [bzip, gcc, ...] [test, train, ref] [raw, blankit, audit, auditmem]"
    echo
    echo "  Note: BLANKIT_PROBE_DBG must be enabled for tracing"
    echo
    exit 1
}

# capture benchmark, folder, and options, if needed
if [ $# -ge 3 ]; then
    declare -A benchmark_folders=()
    declare -A benchmark_args=()
    if [ $3 == "test" ]; then
        for i in ${!benchmark_folders_test[@]}; do
            benchmark_folders[$i]="${benchmark_folders_test[$i]}"
        done
        for i in ${!benchmark_folders_test[@]}; do
            benchmark_args[$i]="${benchmark_args_test[$i]}"
        done
    elif [ $3 == "train" ]; then
        for i in ${!benchmark_folders_train[@]}; do
            benchmark_folders[$i]="${benchmark_folders_train[$i]}"
        done
        for i in ${!benchmark_folders_train[@]}; do
            benchmark_args[$i]="${benchmark_args_train[$i]}"
        done
    elif [ $3 == "ref" ]; then
        for i in ${!benchmark_folders_ref[@]}; do
            benchmark_folders[$i]="${benchmark_folders_ref[$i]}"
        done
        for i in ${!benchmark_folders_ref[@]}; do
            benchmark_args[$i]="${benchmark_args_ref[$i]}"
        done
    else
        usage
    fi
fi
if [ $# == 4 ]; then
    declare -A benchmarks=()
    if [ $4 == "raw" ]; then
        for i in ${!benchmarks_raw[@]}; do
            benchmarks[$i]="${benchmarks_raw[$i]}"
        done
    elif [ $4 == "blankit" ]; then
        for i in ${!benchmarks_embedpin[@]}; do
            benchmarks[$i]="${benchmarks_embedpin[$i]}"
        done
    elif [ $4 == "audit" ] || [ $4 == "auditmem" ]; then
        for i in ${!benchmarks_audit[@]}; do
            benchmarks[$i]="${benchmarks_audit[$i]}"
        done
    else
        usage
    fi
fi


# Build
if [ $# == 1 ] && [ $1 == "build" ]; then
    build

# Trace
elif [ $# == 3 ] && [ $1 == "trace" ]; then
    export BLANKIT_TRACE=1 # value doesn't matter
    build &> ${benchmark_folders[$2]}/build.out
    run $2 &> ${benchmark_folders[$2]}/run.out
    grep "trace_name" ${benchmark_folders[$2]}/run.out | awk '{print $3}' | sort | uniq \
      &> ${benchmark_folders[$2]}/called_funcs_in_$2.txt
    echo
    echo "build output is in ${benchmark_folders[$2]}/build.out"
    echo "run output is in ${benchmark_folders[$2]}/run.out"
    echo "trace output is in ${benchmark_folders[$2]}/called_funcs_in_$2.txt"
    echo

# Run
elif [ $# == 4 ] && [ $1 == "run" ]; then

    if [ $4 == "blankit" ]; then
        export BLANKIT_LAZY=1 # value doesn't matter
        # XXX Need to comment this out when doing eager blanking.
        # Long-term solution is to add an "eager" argument back in.
    fi

    run $2 $3 $4
    exit $retval

# Bad args
else
    usage
fi
