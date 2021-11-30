#!/bin/bash

for f in *_results; do printf "$f "; tail -n10 $f | grep real | awk '{print $2}'; done &> runtimes.out
