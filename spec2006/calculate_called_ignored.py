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

called_funcs = set()
with open('old-string2id_map-Wopin_ref/%s.string2id_map.csv' % benchmark, 'r') as f:
    for line in f:
        line = line.strip()
        if line == "":
            continue
        #called_funcs.update(line.split(';')[1:-1])
        called_funcs.update([x for x in line.split(';')[1:-1] if "plt" not in x])
#print(called_funcs)


ignored_funcs = set()
with open('v4-ignored-lists/%s.ignored_list.txt' % benchmark, 'r') as f:
    for line in f:
        line = line.strip()
        if line == "":
            continue
        ignored_funcs.add(line)
#print(ignored_funcs)

called_funcs_not_ignored = called_funcs - ignored_funcs
not_ignored_ratio = 1.0 * len(called_funcs_not_ignored) / len(called_funcs)
#print(len(called_funcs_not_ignored))
#print(len(called_funcs))
print(not_ignored_ratio)

#print("called_funcs_not_ignored / called_funcs = {} (higher is better)".format(not_ignored_ratio))
#print("called_funcs_not_ignored: {}".format(called_funcs_not_ignored))

#print("called_funcs: {}".format(called_funcs))
#print(len(called_funcs))
#print()
#print("ignored_funcs: {}".format(ignored_funcs))
#print(len(ignored_funcs))
#print()
