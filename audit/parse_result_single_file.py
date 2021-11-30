#!/usr/bin/env python3.5

import sys

class CountSum:
    def __init__(self, c, s):
        self.c = c
        self.s = s
    

result_file = sys.argv[1]

print("Beginning parse for %s" % result_file)
func_to_count_sum = {}
with open("%s" % (result_file), encoding="utf8", errors='ignore') as f:
    for line in f:
        line = line.strip()
        if line == "":
            continue
        line_vec = line.split()
        if line_vec[0] == "AUDIT_NANOSECONDS":
            try:
                func_name = line_vec[1]
                exec_time = int(line_vec[2])
            except:
                continue
            
            if func_name not in func_to_count_sum:
                func_to_count_sum[func_name] = CountSum(0, 0)
            func_to_count_sum[func_name].c += 1
            func_to_count_sum[func_name].s += exec_time

with open("%s.avg" % (result_file), "w") as f:
    for func_name, count_sum in func_to_count_sum.items():
        avg = 1.0 * count_sum.s / count_sum.c
        f.write("%s %f\n" % (func_name, avg))
