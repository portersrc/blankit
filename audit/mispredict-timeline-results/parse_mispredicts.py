#!/usr/bin/env python3.5

print("""\nDisclaimer for the future: We never took these results, because
there was just too much 'length left in the ends' for some benchmarks (meaning
the audit thread would have run for way too long past the program completion).
I  believe it still runs, but the result shows we need to fix upstream problems
in the dataset.\n""")


# hard-coding these now, in the interest of time.
# these are basically just gotten from running the 
start_times = {
    "astar": 1542430281072948000,
    "bzip": 1542424015602762000,
    "gcc": 1542424100545955000,
    "gobmk": 1542424897926034000,
    "h264ref": 1542427212328610000,
    "hmmer": 1542425307058403000,
    "lbm": 1542427265423715000,
    "libquantum": 1542426799343427000,
    "mcf": 1542424147720838000,
    "milc": 1542424430345836000,
    "namd": 1542424890567177000,
    "omnetpp": 1542427488118284000,
    "povray": 1542425154013981000,
    "sjeng": 1542426130934439000,
    "soplex": 1542424963770288000,
    "sphinx": 1542430288006785000,
    "xalancbmk": 1542430876004057000,
}
runtimes = {
    "astar": 6.928000,
    "bzip": 84.899000,
    "gcc": 47.003000,
    "gobmk": 65.811000,
    "h264ref": 52.846000,
    "hmmer": 803.182000,
    "lbm": 222.459000,
    "libquantum": 412.942000,
    "mcf": 282.588000,
    "milc": 458.267000,
    "namd": 7.346000,
    "omnetpp": 2792.951000,
    "povray": 152.608000,
    "sjeng": 668.355000,
    "soplex": 189.076000,
    "sphinx": 583.041000,
    "xalancbmk": 7.175000,
}

avg_func_times = {
    "astar": 51269.8,
    "bzip": 44544.6,
    "gcc": 19520.5,
    "gobmk": 2312.56,
    "h264ref": 18076.7,
    "hmmer": 8497.36,
    "lbm": 1015630,
    "libquantum": 36510.4,
    "mcf": 1973100,
    "milc": 76087.3,
    "namd": 1895.78,
    "omnetpp": 3117.48,
    "povray": 11668.3,
    "sjeng": 5587.39,
    "soplex": 36330.6,
    "sphinx": 2150.82,
    "xalancbmk": 11689.1
}

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
    #"omnetpp",
    "povray",
    "sjeng",
    "soplex",
    "sphinx",
    "xalancbmk"
]




VALGRIND_MULTIPLIER = 20


#
# Note for future:
# This algorithm is a little confusing to follow, but it seems correct or
# close to what we would want if we try this again. The goal is to "simulate"
# the audit thread by checking a timeline of mispredictions against what
# we suspect would be the time for an audit thread to run it in valgrind.
# It's also worth pointing out that because of the difficulty in matching
# function names and other problems, this current implementation naively 
# hard-codes the average function time for a benchmark (yes, over all valgrind
# function times for that benchmark). This is clearly an inaccurate simulation.
# In fact, in the paper, we "simulated" just with napkin math by figuring
# out the total number of mispredictions and 
# the mispredictions tried to make an estimate
#
#bmark = "bzip"
for bmark in benchmarks:
    print("Checking %s" % bmark)
    with open("%s.mispredict_timeline" % bmark) as f:

        ends = [0]
        i = 0
        audit_time = avg_func_times["%s" % bmark]
        #audit_time = avg_func_times["%s" % bmark] * VALGRIND_MULTIPLIER

        for line in f:
            line = line.strip().split()
            func_name = line[0]
            timestamp = int(line[1])

            while i < len(ends) and timestamp > ends[i]:
                i += 1

            if i == len(ends):
                audit_end = timestamp + audit_time
                ends.append(audit_end)
            else:
                audit_end = ends[-1] + audit_time
                ends.append(audit_end)

    print("  length left in ends: %d" % len(ends))

print("clean exit")


