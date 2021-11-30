#!/usr/bin/env python3

max_funcs = 0
max_line  = []
with open("string2id_map.csv") as f:
    for line in f:
        line = line.strip().split(';')[1:-1]
        if len(line) > max_funcs:
            max_funcs = len(line)
            max_line  = line
assert len(max_line) == max_funcs
print(' '.join(max_line))
print("size: %d" % max_funcs)
