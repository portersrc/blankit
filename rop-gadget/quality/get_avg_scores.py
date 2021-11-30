#!/usr/bin/env python3
import sys
import os


#
# 2019.02.20: Wrote this script for Usenix Security 2019. I believe this script
# just parses the avg gadget scores for each of the call chains for a given
# benchmark. Then it calculates the average over all of those and reports it.
#


benchmarks = [
	'401.bzip2',
	'403.gcc',
	'429.mcf',
	'433.milc',
	'444.namd',
	'445.gobmk',
	'450.soplex',
	'453.povray',
	'456.hmmer',
	'458.sjeng',
	'462.libquantum',
	'464.h264ref',
	'470.lbm',
	'471.omnetpp',
	'473.astar',
	'482.sphinx3',
	'483.xalancbmk',
]

for benchmark in benchmarks:
	#print(os.listdir('401.bzip2'))

    total = 0.0
    count = 0
    
    files = os.listdir(benchmark)
    for filename in files:
        full_name = benchmark + '/' + filename
        #with open(sys.argv[1]) as f:
        with open(full_name) as f:
            for line in f:
                line = line.strip()
                if "Average gadget score" in line:
                    line_vec = line.split(':')
                    score = line_vec[1].strip()
                    if score == "NaN":
                        # we could do something here... im not sure i trust these
                        # cases enough, though, to claim that we had 0 gadgets in the
                        # call chain
                        pass
                    else:
                        score = float(score)
                        total += score
                        count += 1
    avg = total / count
    print("%s %f" % (benchmark, avg))



