#!/bin/bash
for f in `ls`; do printf $f" "; grep "RTN name:.*not adding probe" $f | wc -l; done
