#!/usr/bin/env python3


#
# This program should print nothing. That is, all funcs should be in the glibc
# routine list.
#

print()

libc_funcs = set()
with open("../libc/glibc_routine_list.txt") as f:
    for line in f:
        libc_funcs.add(line.strip())

failed = False
with open("list.txt") as f:
    for line in f:
        func = line.strip()
        if func not in libc_funcs:
            failed = True
            print(func)

if failed:
    print("Failed: printed CVE funcs fall outside of the glibc routine list\n")
    exit(1)
print("Success: No CVE funcs fall outside of the glibc routine list")

print()
