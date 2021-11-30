#!/bin/bash

for f in *_results; do echo $f; head -n20 $f | grep audit-start-time; done
