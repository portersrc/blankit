#!/usr/bin/env python3.5
import sys
import re

load_time  = 0
runtime    = 0

with open(sys.argv[1], 'r') as f:
    for line in f:
        line = line.strip().split()
        benchmark = line[0]
        raw_time  = line[1]

        m = re.match(r"(.*)(m)(.*)(s)", raw_time)
        if m:
            minutes = int(m.group(1))
            #print(minutes)
            seconds = float(m.group(3))
            #print(m.group(2))
            runtime = minutes * 60 + seconds

            print("%s %f" % (benchmark, runtime))
