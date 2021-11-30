#!/usr/bin/env python3



import sys

def usage():
    print()
    print("Usage:")
    print("  {} [401.bzip2, 403.gcc, ...] ".format(sys.argv[0]))
    print()
    exit(1)

if len(sys.argv) != 2:
    usage()

benchmark = sys.argv[1]
exposed_funcs = set()

with open('v4-longest-call-chains/%s.longest_call_chain.txt' % benchmark, 'r') as f:
    for line in f:
        line = line.strip()
        if line == "":
            continue
        line = line.split(',')
        for f in line:
            exposed_funcs.add(f)


with open('v4-ignored-lists/%s.ignored_list.txt' % benchmark, 'r') as f:
    for line in f:
        line = line.strip()
        if line == "":
            continue
        exposed_funcs.add(line)


print(len(exposed_funcs))
