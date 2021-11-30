#!/bin/bash

dir=test_cmd_dir
for i in {1..100}; do
    mkdir -p ${dir}.${i}
    pushd ${dir}.${i}
    echo "hello world" > hello.txt
    cat hello.txt
    popd
    rm -r ${dir}.${i}
done
