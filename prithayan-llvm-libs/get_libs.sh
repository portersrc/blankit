#!/bin/bash

P=/home/prithayan/project/llvm/build/lib

arr=(
    LLVMembedDtree_final_1.so
    LLVMembedDtree_opti0.so
    LLVMembedDtree.so
    LLVMpathPredInstrument.so
)

for x in ${arr[@]}; do
    scp ashwa:$P/$x .
done
