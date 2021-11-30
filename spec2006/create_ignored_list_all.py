#!/usr/bin/env python3
import sys

uses_libc = set([
    "401.bzip2",
    #403.gcc
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

with open("%s.ignored_list_all.txt" % benchmark, "w") as f:
    for func in sorted(exposed_funcs):
        f.write(func+'\n')
