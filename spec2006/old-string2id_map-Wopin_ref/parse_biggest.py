#!/usr/bin/env python3.5
import sys

def usage():
    print()
    print("Usage:")
    print("  {} [401.bzip2, 403.gcc, ...]".format(sys.argv[0]))
    print()
    exit(1)

benchmark = sys.argv[1]

max_len = 0
max_line = []
with open("%s.string2id_map.csv" % benchmark) as f:
    for line in f:
        line = line.strip().split(';')[1:-1]
        if len(line) > max_len:
            max_len = len(line)
            max_line = line
print("%d %s" % (max_len, ','.join(max_line)))
