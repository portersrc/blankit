#!/usr/bin/env python3.5

import sys

# The purpose of this parser is twofold:
# 1. Take the output from some drive-blankit result and produce a cleaner
# version with just two-columns:
#   func-name timestamp
# 2. Dump the ratio of mispredicts to correct predicts for validating
# runtime accuracy.

# Assumes the input files are called <bmark_name>_results
# e.g. bzip2_results



benchmarks = [
    "astar",
    "bzip",
    "gcc",
    "gobmk",
    "h264ref",
    "hmmer",
    "lbm",
    "libquantum",
    "mcf",
    "milc",
    "namd",
    "omnetpp",
    "povray",
    "sjeng",
    "soplex",
    "sphinx",
    "xalancbmk"
]

#benchmark       = sys.argv[1]
for bmark in benchmarks:
    benchmark = bmark
    input_filename  = benchmark+"_results"
    output_filename = benchmark+".mispredict_timeline"

    num_correct_predicts = 0
    num_mispredicts      = 0

    with open(output_filename, "w") as f_out:
        with open(input_filename) as f:
            for line in f:
                line = line.strip()
                if line == "":
                    continue
                line_vec = line.split()
                if line_vec[0] == "audit-correct-predict":
                    num_correct_predicts += 1
                elif line_vec[0] == "audit-mispredict":
                    num_mispredicts += 1
                    func_name = line_vec[1]
                    timestamp = int(line_vec[2])
                    f_out.write("%s %d\n" % (func_name, timestamp))

    print("%s" % benchmark)
    print("  num_correct_predicts: %d" % num_correct_predicts)
    print("  num_mispredicts: %d" % num_mispredicts)
    print("  prediction rate: %f" % (1.0 * num_correct_predicts / (num_correct_predicts + num_mispredicts)))
