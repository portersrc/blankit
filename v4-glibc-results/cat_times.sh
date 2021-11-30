#!/bin/bash

set -e

for i in $(ls *.times); do
    t=$(./parse_time.py $i)
    ii=$(echo $i | cut -d '_' -f 1)
    echo "$ii $t"
done
