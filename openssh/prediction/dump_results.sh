#!/bin/bash

for f in `ls */learnModel_output`; do
    echo "================="
    echo "- "$f
    echo "================="
    echo
    cat $f
    echo
    echo
    echo
    echo
done
