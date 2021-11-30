#!/bin/bash

dir=test_cmd_dir.${1}
for i in {1..100}; do
    mkdir -p $dir
    pushd $dir
    echo "hello world" > hello.txt
    cat hello.txt
    popd
done
rm -r $dir
