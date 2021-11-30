#!/usr/bin/env python3
import sys
import re

load_time  = 0
runtime    = 0

with open(sys.argv[1], 'r') as f:
    for line in f:
        line = line.strip()
        """
        if 'm' in line or 's' in line:
            minutes = line.split('m')[0]
            seconds = line.split('s')[0]
            total_time += int(minutes) * 60
            total_time += float(seconds)
        """

        m = re.match(r"(.*)(m)(.*)(s)", line)
        if m:
            minutes = int(m.group(1))
            #print(minutes)
            seconds = float(m.group(3))
            #print(m.group(2))
            runtime = minutes * 60 + seconds
        else:
            load_time += int(line) / 1000000.0

total_time = runtime - load_time
print(total_time)
