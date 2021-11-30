#!/usr/bin/env python3
import sys

uses_libc = set([
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
])

uses_libm = set([
    "433.milc",
    "444.namd",
    "445.gobmk",
    "450.soplex",
    "453.povray",
    "456.hmmer",
    "462.libquantum",
    "464.h264ref",
    "470.lbm",
    "471.omnetpp",
    "473.astar",
    "482.sphinx3",
    "483.xalancbmk"
])

uses_libgcc = set([
    "444.namd",
    "450.soplex",
    "453.povray",
    "462.libquantum",
    "471.omnetpp",
    "473.astar",
    "483.xalancbmk"
])

uses_libstdcpp = set([
    "444.namd",
    "450.soplex",
    "453.povray",
    "471.omnetpp",
    "473.astar",
    "483.xalancbmk"
])

lib_to_uses = {
    "libc": uses_libc,
    "libm": uses_libm,
    "libgcc": uses_libgcc,
    "libstdcpp": uses_libstdcpp
}



LIBC_LF      = 1743
LIBM_LF      = 357
LIBGCC_LF    = 130
LIBSTDCPP_LF = 3393
total_linked_funcs_map = {
    "401.bzip2":      LIBC_LF,
    "403.gcc":        LIBC_LF,
    "429.mcf":        LIBC_LF,
    "433.milc":       LIBC_LF + LIBM_LF,
    "444.namd":       LIBC_LF + LIBM_LF + LIBGCC_LF + LIBSTDCPP_LF,
    "445.gobmk":      LIBC_LF + LIBM_LF,
    "450.soplex":     LIBC_LF + LIBM_LF + LIBGCC_LF + LIBSTDCPP_LF,
    "453.povray":     LIBC_LF + LIBM_LF + LIBGCC_LF + LIBSTDCPP_LF,
    "456.hmmer":      LIBC_LF + LIBM_LF,
    "458.sjeng":      LIBC_LF,
    "462.libquantum": LIBC_LF + LIBM_LF + LIBGCC_LF,
    "464.h264ref":    LIBC_LF + LIBM_LF,
    "470.lbm":        LIBC_LF + LIBM_LF,
    "471.omnetpp":    LIBC_LF + LIBM_LF + LIBGCC_LF + LIBSTDCPP_LF,
    "473.astar":      LIBC_LF + LIBM_LF + LIBGCC_LF + LIBSTDCPP_LF,
    "482.sphinx3":    LIBC_LF + LIBM_LF,
    "483.xalancbmk":  LIBC_LF + LIBM_LF + LIBGCC_LF + LIBSTDCPP_LF,
}

def get_percent_reduction(num_exposed_funcs):
    total = total_linked_funcs_map[benchmark]
    return (1.0 * total - num_exposed_funcs) / total * 100

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



ignored_trampoline_pin = {
    "libc": set(),
    "libm": set(),
    "libgcc": set(),
    "libstdcpp": set()
}



for lib, uses in lib_to_uses.items():
    if benchmark in uses:
        with open("ignored_due_to_trampoline_or_pin/%s.txt.ignored_funcs" % lib) as f:
            for line in f:
                line = line.strip()
                if line == "":
                    continue
                exposed_funcs.add(line)
        suffix = "-%s" % lib
        if lib == "libc":
            suffix =  ""
        with open("v4-ignored-lists%s/%s.ignored_list.txt" % (suffix, benchmark)) as f:
            for line in f:
                line = line.strip()
                if line == "":
                    continue
                exposed_funcs.add(line)

num_exposed_funcs = len(exposed_funcs)
percent_reduction = get_percent_reduction(num_exposed_funcs)
print("%d %s" % (num_exposed_funcs, percent_reduction))
