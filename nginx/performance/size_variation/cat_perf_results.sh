#!/bin/bash

#"CPUs utilized"
#"insn per cycle"
#"page-faults"

METRIC="CPUs utilized"
echo
echo ${METRIC}
for T in {1,10,100}; do
    BUF_PIN=()
    BUF_NOPIN=()
    for FOLDER in {1-perf,2-perf,3-perf}; do
        rv=$(grep "${METRIC}" ${FOLDER}/nginx-perf.${T}mb.pin.out | awk '{print $5}')
        BUF_PIN+=($rv)
        rv=$(grep "${METRIC}" ${FOLDER}/nginx-perf.${T}mb.nopin.out | awk '{print $5}')
        BUF_NOPIN+=($rv)
    done
    echo "nopin pin (${T}mb)"
    for i in {0..2}; do
        echo ${BUF_NOPIN[i]}" "${BUF_PIN[i]}
    #    echo $v
    done
done


METRIC="insn per cycle"
echo
echo ${METRIC}
for T in {1,10,100}; do
    BUF_PIN=()
    BUF_NOPIN=()
    for FOLDER in {1-perf,2-perf,3-perf}; do
        rv=$(grep "${METRIC}" ${FOLDER}/nginx-perf.${T}mb.pin.out | awk '{print $4}')
        BUF_PIN+=($rv)
        rv=$(grep "${METRIC}" ${FOLDER}/nginx-perf.${T}mb.nopin.out | awk '{print $4}')
        BUF_NOPIN+=($rv)
    done
    echo "nopin pin (${T}mb)"
    for i in {0..2}; do
        echo ${BUF_NOPIN[i]}" "${BUF_PIN[i]}
    #    echo $v
    done
done


METRIC="page-faults"
echo
echo ${METRIC}
for T in {1,10,100}; do
    BUF_PIN=()
    BUF_NOPIN=()
    for FOLDER in {1-perf,2-perf,3-perf}; do
        rv=$(grep "${METRIC}" ${FOLDER}/nginx-perf.${T}mb.pin.out | awk '{print $1}')
        BUF_PIN+=($rv)
        rv=$(grep "${METRIC}" ${FOLDER}/nginx-perf.${T}mb.nopin.out | awk '{print $1}')
        BUF_NOPIN+=($rv)
    done
    echo "nopin pin (${T}mb)"
    for i in {0..2}; do
        echo ${BUF_NOPIN[i]}" "${BUF_PIN[i]}
    #    echo $v
    done
done
echo
