#!/usr/bin/env python3

libc_funcs   = set()
called_funcs = set()

with open('../libc/glibc_routine_list.txt') as f:
    for line in f:
        line = line.strip()
        if line in libc_funcs:
            print("what in the fuck: %s" % line)
        libc_funcs.add(line)

with open('called_funcs.txt') as f:
    for line in f:
        line = line.strip()
        called_funcs.add(line)

for func in libc_funcs:
    if func not in called_funcs:
        #print(func)
        pass
