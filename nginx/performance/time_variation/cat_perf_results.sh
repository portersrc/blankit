#!/bin/bash

#"CPUs utilized"
#"insn per cycle"
#"page-faults"

METRIC="CPUs utilized"
echo
echo ${METRIC}
for T in {3,30,300}; do
    BUF_PIN=()
    BUF_NOPIN=()
    for FOLDER in {1-perf,2-perf,3-perf}; do
        #echo ${FOLDER}/nginx-perf.${T}s.${PNP}.out
        rv=$(grep "${METRIC}" ${FOLDER}/nginx-perf.${T}s.pin.out | awk '{print $5}')
        BUF_PIN+=($rv)
        rv=$(grep "${METRIC}" ${FOLDER}/nginx-perf.${T}s.nopin.out | awk '{print $5}')
        BUF_NOPIN+=($rv)
    done
    echo "nopin pin (${T}s)"
    for i in {0..2}; do
        echo ${BUF_NOPIN[i]}" "${BUF_PIN[i]}
    #    echo $v
    done
done


METRIC="insn per cycle"
echo
echo ${METRIC}
for T in {3,30,300}; do
    BUF_PIN=()
    BUF_NOPIN=()
    for FOLDER in {1-perf,2-perf,3-perf}; do
        #echo ${FOLDER}/nginx-perf.${T}s.${PNP}.out
        rv=$(grep "${METRIC}" ${FOLDER}/nginx-perf.${T}s.pin.out | awk '{print $4}')
        BUF_PIN+=($rv)
        rv=$(grep "${METRIC}" ${FOLDER}/nginx-perf.${T}s.nopin.out | awk '{print $4}')
        BUF_NOPIN+=($rv)
    done
    echo "nopin pin (${T}s)"
    for i in {0..2}; do
        echo ${BUF_NOPIN[i]}" "${BUF_PIN[i]}
    #    echo $v
    done
done


METRIC="page-faults"
echo
echo ${METRIC}
for T in {3,30,300}; do
    BUF_PIN=()
    BUF_NOPIN=()
    for FOLDER in {1-perf,2-perf,3-perf}; do
        #echo ${FOLDER}/nginx-perf.${T}s.${PNP}.out
        rv=$(grep "${METRIC}" ${FOLDER}/nginx-perf.${T}s.pin.out | awk '{print $1}')
        BUF_PIN+=($rv)
        rv=$(grep "${METRIC}" ${FOLDER}/nginx-perf.${T}s.nopin.out | awk '{print $1}')
        BUF_NOPIN+=($rv)
    done
    echo "nopin pin (${T}s)"
    for i in {0..2}; do
        echo ${BUF_NOPIN[i]}" "${BUF_PIN[i]}
    #    echo $v
    done
done
echo
