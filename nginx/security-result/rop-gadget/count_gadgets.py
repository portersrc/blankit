#!/usr/bin/env python3
import sys
import os

benchmark="nginx"

MAX_ADDR_LIBC       = 0x159e61
MAX_ADDR_LIBCRYPT   = 0x67f0
MAX_ADDR_LIBDL      = 0x1998
MAX_ADDR_LIBPCRE    = 0x5062c
MAX_ADDR_LIBPTHREAD = 0x12adc
MAX_ADDR_LIBZ       = 0x12818




avail_tools = set([
    "count",
    "ignored_high_addr",
    "rop_gadgets_by_call_chains"
])

def usage_and_exit():
    print("\nUsage:")
    print("\t%s [count, ignored_high_addr, rop_gadgets_by_call_chains]\n"
          % sys.argv[0])
    print("\n\n")
    exit(1)


class OffsetSize:
    def __init__(self, offset, size):
        self.m = {}
        self.m[offset] = size
        self.offset = offset
        self.size = size

    def add(self, offset, size):
        if offset not in self.m:
            self.m[offset] = size

def parse_lib_files(lib):

    func_id = 1
    with open(lib.routine_list, "r") as f:
        for line in f:
            func = line.strip()
            lib.func_to_id[func] = func_id
            func_id += 1

    with open(lib.name_offset_size_file, "r") as f:
        for line in f:
            (func, offset, size) = line.strip().split()
            offset = int(offset, 16)
            size = int(size)
            if offset > lib.max_addr:
                continue
            idx_of_at = func.find("@")
            if idx_of_at != -1:
                func = func[:idx_of_at]

            if func in lib.func_to_offset_size:
                lib.func_to_offset_size[func].add(offset, size)
            else:
                lib.func_to_offset_size[func] = OffsetSize(offset, size)

    with open(lib.rop_gadgets_file, "r") as f:
        for line in f:
            addr = line.strip().split()[0]
            lib.gadgets.append(int(addr, 16))
            lib.gadget_instructions[int(addr, 16)] = line.strip()

    try:
        ignored_funcs_set = set()
        with open(lib.ignored_trampoline_pin_file, "r") as f:
            for line in f:
                ignored_funcs_set.add(line.strip())
        with open(lib.ignored_list_file, "r") as f:
            for line in f:
                ignored_funcs_set.add(line.strip())
        lib.ignored_funcs = list(ignored_funcs_set)
    except:
        # not all benchmarks have all libraries, hence open can fail
        pass

def parse_call_chains():
    with open(call_chains_file) as f:
        for line in f:
            line = line.strip()
            if line == "":
                continue
            line = line.split(';')
            funcs_list = line[1:-1]
            # only append if the call chain has at least one function
            if len(funcs_list) > 0:
                call_chains.append(funcs_list)
            #call_chains.append(funcs_list)


# Get the number of gadgets in some list of functions
def get_num_gadgets(funcs, lib):
    num_gadgets = 0
    for func in funcs:
        if func not in lib.func_to_id:
            #print("ignoring %s" % func)
            continue
        #print(func)
        func_id = lib.func_to_id[func]
        if func_id in lib.func_id_to_num_gadgets:
            num_gadgets += lib.func_id_to_num_gadgets[func_id]
        else:
            #print("no gadgets found for %s" % func)
            pass
    return num_gadgets

# Get the gadget instructions (as a list) in some list of functions
def get_gadget_instructions(funcs, lib):
    gadget_instructions = []
    for func in funcs:
        if func not in lib.func_to_id:
            continue
        func_id = lib.func_to_id[func]
        if func_id in lib.func_id_to_gadget_instructions:
            gadget_instructions.extend(lib.func_id_to_gadget_instructions[func_id])
        else:
            pass
    return gadget_instructions

def map_func_addr_to_id(lib):
    for func, offset_size in sorted(lib.func_to_offset_size.items()):
        offset = offset_size.offset
        size   = offset_size.size
        for i in range(size):
            addr = offset + i
            # If "func" is some weird name (e.g. has @@GLIBC in it), then it
            # won't be in func_to_id, and so it won't get added to func_addr_to_id
            if func in lib.func_to_id:
                lib.func_addr_to_id[addr] = lib.func_to_id[func]

def map_func_id_to_gadgets(lib):
    #print("len of func addr to id for library {}: {}".format(lib.name, hex(len(lib.func_addr_to_id))))
    for addr in lib.gadgets:
        if addr > len(lib.func_addr_to_id):
            #print("ignoring addr({}) > len(func_addr_to_i)({})".format(hex(addr), hex(len(func_addr_to_id))))
            lib.ignored_high_addr += 1
            continue
        func_id = lib.func_addr_to_id[addr]
        if func_id not in lib.func_id_to_num_gadgets:
            lib.func_id_to_num_gadgets[func_id] = 1
            lib.func_id_to_gadget_instructions[func_id] = [lib.gadget_instructions[addr]]
        else:
            lib.func_id_to_num_gadgets[func_id] += 1
            lib.func_id_to_gadget_instructions[func_id].append(lib.gadget_instructions[addr])


class LibDetails:
    def __init__(self, name, max_addr):
        self.name = name
        self.max_addr = max_addr
        self.rop_gadgets_file = "lib-rop-gadgets/%s-rop-gadgets.out" % name
        self.routine_list = "../../complete-lists/%s_routine_list.txt" % name
        self.name_offset_size_file = "offset_and_size/%s_name_offset_size.out" % name
        self.ignored_list_file = "../ignored_lists/%s.ignored_list.txt" % (name)
        self.ignored_trampoline_pin_file = "../ignored_due_to_trampoline_or_pin/%s-ignored-pin-tramp.txt" % (self.name)
        self.func_to_id = {}
        self.func_to_offset_size = {}
        self.gadgets = []
        self.gadget_instructions = {} # addr -> gadget instruction
        self.func_addr_to_id = [0] * (max_addr + 1)
        self.func_id_to_num_gadgets = {}
        self.func_id_to_gadget_instructions = {}
        self.ignored_funcs = []
        self.ignored_high_addr = 0



call_chains   = []

call_chains_file = "../../prediction/test-wiki/string2id_map.csv"

libs = [
    LibDetails("libc", MAX_ADDR_LIBC),
    LibDetails("libcrypt", MAX_ADDR_LIBCRYPT),
    LibDetails("libdl", MAX_ADDR_LIBDL),
    LibDetails("libpcre", MAX_ADDR_LIBPCRE),
    LibDetails("libpthread", MAX_ADDR_LIBPTHREAD),
    LibDetails("libz", MAX_ADDR_LIBZ)
]

parse_call_chains()
for lib in libs:
    parse_lib_files(lib)
    map_func_addr_to_id(lib)
    map_func_id_to_gadgets(lib)

if len(sys.argv) != 2:
    usage_and_exit()

which_tool = sys.argv[1]
if which_tool not in avail_tools:
    usage_and_exit()

#
# This is a way to check the ignored high-addr
#
if which_tool == "ignored_high_addr":
    for lib in libs:
        print("{} {}".format(lib.name, lib.ignored_high_addr))
    exit(0)





# get the total number of ROP gadgets from ignored functions across
# all libraries
ignored_counts = 0
ignored_gadget_instructions = []
for lib in libs:
    ignored_counts += get_num_gadgets(lib.ignored_funcs, lib)
    ignored_gadget_instructions.extend(get_gadget_instructions(lib.ignored_funcs, lib))

# get the total number of ROP gadgets in each call chain. sloppily do this
# for each lib so that each function eventually is checked against its home
# library
call_chain_counts = []
call_chain_gadget_instructions = []
for call_chain in call_chains:
    lib_counts = 0
    lib_gadget_instructions = []
    for lib in libs:
        lib_counts += get_num_gadgets(call_chain, lib)
        lib_gadget_instructions.extend(get_gadget_instructions(call_chain, lib))
    call_chain_counts.append(lib_counts)
    call_chain_gadget_instructions.append(lib_gadget_instructions)
# if multiple chains tie for max, we don't do a bake-off
idx_of_max = call_chain_counts.index(max(call_chain_counts))
result = max(call_chain_counts) + ignored_counts
result_instructions = call_chain_gadget_instructions[idx_of_max] + ignored_gadget_instructions
print("{} {}".format(benchmark,result))



# XXX This is disabled so I don't create extra files that have to be
# deleted. This really should be made as an option when running this
# tool
# --- update: not sure what this is used for though?
"""
with open("%s.gadget_instructions" % benchmark, "w") as f:
    for gadget_instruction in result_instructions:
        f.write(gadget_instruction+"\n")
"""






# Write out the gadget instructions for all the call chains.
# Reason: we want to get the gadget quality for each call chain
# for each call chain
if which_tool == "rop_gadgets_by_call_chains":

    try:
        os.mkdir("rop-gadgets-by-call-chain")
    except FileExistsError:
        pass

    i = 0
    for gadget_instructions in call_chain_gadget_instructions:
        # open a new file (using an index to keep it unique)
        with open("rop-gadgets-by-call-chain/%s.%d.gadget_instructions" % (benchmark, i), "w") as f:
            # and write out the gadget instruction in that call chain
            for gadget_instruction in gadget_instructions:
                f.write(gadget_instruction+"\n")
        i += 1

    # also write out the ignored gadget instructions in their own file.
    with open("rop-gadgets-by-call-chain/%s.ignored_gadget_instructions" % (benchmark), "w") as f:
        for ignored_gadget_instruction in ignored_gadget_instructions:
            f.write(ignored_gadget_instruction+"\n")
