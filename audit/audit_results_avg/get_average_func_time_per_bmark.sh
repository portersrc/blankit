#!/bin/bash
for f in *.avg; do
    #printf "$f "
    #cat $f | awk '{ sum += $2; n++ } END { if (n > 0) print sum / n * 20; }'
    echo $f
    cat $f | awk '{print $2}'
    echo
done

