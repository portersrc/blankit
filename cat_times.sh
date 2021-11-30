#!/bin/bash

set -e

CURDIR=`dirname "$(readlink -f "$0")"`

for i in $(ls *.times); do
    t=`${CURDIR}/parse_time.py $i`
    ii=$(echo $i | cut -d '_' -f 1)
    echo "$ii $t"
done
