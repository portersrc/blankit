#!/usr/bin/env python3.6
import sys


benchmarks = [
    "401.bzip2",
    "403.gcc",
    "429.mcf",
    "433.milc",
    "444.namd",
    "445.gobmk",
    "450.soplex",
    "453.povray",
    "456.hmmer",
    "458.sjeng",
    "462.libquantum",
    "464.h264ref",
    "470.lbm",
    "471.omnetpp",
    "473.astar",
    "482.sphinx3",
    "483.xalancbmk"
]



call_chains      = {}
call_chains_min  = {}
running_sum      = 0
running_cnt      = 0

def parse_call_chains():
    global call_chains, call_chains_min, running_sum, running_cnt
    with open(call_chains_file) as f:
        for line in f:
            line = line.strip()
            if line == "":
                continue
            line = line.split(';')
            funcs_list = line[1:-1]
            # only append if the call chain has at least one function
            if len(funcs_list) > 0:
                first_func = funcs_list[0]
                if first_func not in call_chains:
                    call_chains[first_func] = set()
                    call_chains_min[first_func] = sys.maxsize
                call_chains[first_func] = call_chains[first_func].union(funcs_list)
                if len(funcs_list) < call_chains_min[first_func]:
                    call_chains_min[first_func] = len(funcs_list)
    for k in call_chains:
        increase = len(call_chains[k]) * 1.0 / call_chains_min[k]
        running_sum += increase
        running_cnt += 1
        #print("%s:" % k)
        #print("\tset length: %d" % len(call_chains[k]))
        #print("\tmin length: %d" % call_chains_min[k])
        #print("\tincrease:   %f" % increase)
        #print()


for benchmark in benchmarks:

    call_chains_file = "../spec2006/Wopin_ref/%s/string2id_map.csv" % benchmark

    parse_call_chains()
    #sys.exit(42)

print(running_sum * 1.0 / running_cnt)
