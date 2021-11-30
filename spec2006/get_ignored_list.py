#!/usr/bin/env python3
import sys

LIBC_ROUTINE_LIST      = "../libc/glibc_routine_list.txt"
LIBM_ROUTINE_LIST      = "../libm/libm_routine_list.txt"
LIBGCC_ROUTINE_LIST    = "../libgcc/libgcc_routine_list.txt"
LIBSTDCPP_ROUTINE_LIST = "../libstdcpp/libstdcpp_routine_list.txt"

routine_list_map = {
    "libc": LIBC_ROUTINE_LIST,
    "libm": LIBM_ROUTINE_LIST,
    "libgcc": LIBGCC_ROUTINE_LIST,
    "libstdcpp": LIBSTDCPP_ROUTINE_LIST
}
list_suffix_map = {
    "libc": "",
    "libm": "_libm",
    "libgcc": "_libgcc",
    "libstdcpp": "_libstdcpp"
}

def usage():
    print()
    print("Usage:")
    print("  {} [401.bzip2, 403.gcc, ...] [libc, libm, libgcc, libstdcpp]".format(sys.argv[0]))
    print()
    exit(1)

if len(sys.argv) != 3:
    usage()

benchmark = sys.argv[1]
lib       = sys.argv[2]

routine_list = routine_list_map[lib]
list_suffix  = list_suffix_map[lib]
funcs = []
approved = set()


with open(routine_list) as f:
    for line in f:
        funcs.append(line.strip())

with open("v4-approved-lists-%s/%s.approved_list%s.txt" % (list_suffix[1:],benchmark, list_suffix)) as f:
    for line in f:
        approved.add(line.strip())

for f in funcs:
    if f not in approved:
        print(f)
