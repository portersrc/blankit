#!/usr/bin/env python3
import sys

libs = [
    "libc",
    #"libcrypt",
    #"libdl",
    #"libpcre",
    #"libpthread",
    #"libz"
]



exposed_funcs = set()
partially_exposed_funcs = set()


with open('../../prediction/test-wiki/string2id_map.csv') as f:
    for line in f:
        line = line.strip()
        if line == "":
            continue
        line = line.split(';')[1:-1]
        for f in line:
            partially_exposed_funcs.add(f)



for lib in libs:
    with open("../ignored_due_to_trampoline_or_pin/%s-ignored-pin-tramp.txt" % lib) as f:
        for line in f:
            line = line.strip()
            if line == "":
                continue
            exposed_funcs.add(line)
    with open("../ignored_lists/%s.ignored_list.txt" % lib) as f:
        for line in f:
            line = line.strip()
            if line == "":
                continue
            exposed_funcs.add(line)


exposed_count = 0
partially_exposed_count = 0
with open("list.txt") as f:
    for line in f:
        func = line.strip()
        if func in exposed_funcs:
            exposed_count += 1
        elif func in partially_exposed_funcs:
            partially_exposed_count += 1

total_exposed = exposed_count + partially_exposed_count
total_cve_funcs = 47 # pre-computed, known

#print("total cve funcs: %d" % (total_cve_funcs))
#print("exposed count: %d" % (exposed_count))
#print("partially exposed count: %d" % (partially_exposed_count))
#
#print("percent reduction: ( (%d - %d) / (1.0 * %d) ) * 100" % (total_cve_funcs, total_exposed, total_cve_funcs))
#print("                   = %f" % ((total_cve_funcs - total_exposed) / (1.0 * total_cve_funcs) * 100))
print("%% glibc CVE reduction: %f" % ((total_cve_funcs - total_exposed) / (1.0 * total_cve_funcs) * 100))
