#!/usr/bin/env python3.6

# 
# This script is for getting the following information (email from santosh):
#
# 1. The number of glibc functions called by SPEC: min, max and avg and the
#    number of call edges of the corresponding static graphs
# 2. Static call chain depths called into glibc by SPEC
# 3. Number of divergences (branches) guarding those call chains - dont know if
#    you have this number - Girish had written a pass about it - so get it from him
#



dyn_call_lists = [
    "../spec2006/Wopin/445.gobmk/called_funcs_in_gobmk.txt",
    "../spec2006/Wopin/470.lbm/called_funcs_in_lbm.txt",
    "../spec2006/Wopin/403.gcc/called_funcs_in_gcc.txt",
    "../spec2006/Wopin/456.hmmer/called_funcs_in_hmmer.txt",
    "../spec2006/Wopin/453.povray/called_funcs_in_povray.txt",
    "../spec2006/Wopin/433.milc/called_funcs_in_milc.txt",
    "../spec2006/Wopin/473.astar/called_funcs_in_astar.txt",
    "../spec2006/Wopin/458.sjeng/called_funcs_in_sjeng.txt",
    "../spec2006/Wopin/464.h264ref/called_funcs_in_h264ref.txt",
    "../spec2006/Wopin/450.soplex/called_funcs_in_soplex.txt",
    "../spec2006/Wopin/401.bzip2/called_funcs_in_bzip.txt",
    "../spec2006/Wopin/462.libquantum/called_funcs_in_libquantum.txt",
    "../spec2006/Wopin/483.xalancbmk/called_funcs_in_xalancbmk.txt",
    "../spec2006/Wopin/429.mcf/called_funcs_in_mcf.txt",
    "../spec2006/Wopin/471.omnetpp/called_funcs_in_omnetpp.txt",
    "../spec2006/Wopin/482.sphinx3/called_funcs_in_sphinx.txt",
    "../spec2006/Wopin/444.namd/called_funcs_in_namd.txt",
]


# (1)
num_glibc_funcs_called_by_spec   = []
num_nodes_of_static_graphs       = []
num_edges_of_static_graphs       = []

# (2) 
max_depths_of_static_call_chains = []

# (3)
num_non_divergent_functions      = []
num_key_errors                    = []



static_cg = {}
non_divergent_functions = set()


def dfs(worklist):

    global key_errors
    visited = set()
    max_depth = 0
    num_edges = 0
    num_non_divergent = 0
    key_errors = 0

    while len(worklist) > 0:

        (func, depth) = worklist.pop()

        if depth > max_depth:
            max_depth = depth

        if func in visited:
            continue

        visited.add(func)
        if func in non_divergent_functions:
            num_non_divergent += 1

        if func not in static_cg:
            key_errors += 1
            continue

        num_edges += len(static_cg[func])
        worklist.extend([(func_name, depth+1) for func_name in static_cg[func]])

    max_depths_of_static_call_chains.append(max_depth)
    num_edges_of_static_graphs.append(num_edges)
    num_nodes_of_static_graphs.append(len(visited))
    num_non_divergent_functions.append(num_non_divergent)
    num_key_errors.append(key_errors)

    print('depth:     {}'.format(depth))
    print('max_depth: {}'.format(max_depth))




def min_max_avg(l):
    return (min(l), max(l), sum(l) * 1.0 / len(num_glibc_funcs_called_by_spec))









# Create the static call graph
with open("staticReachableFuncs") as f:
    for line in f:
        line = line.strip()
        line = line[:-1] # chop off trailing comma
        line_vec = line.split(':')
        func_name = line_vec[0]
        static_reach_funcs = line_vec[2].split(',')
        static_cg[func_name] = static_reach_funcs

# Store the non-divergent functions
with open("non_divFunctionsList") as f:
    for line in f:
        func = line.strip().split(':')[0]
        non_divergent_functions.add(func)

# For each benchmark's dynamic call list for glibc, gather metrics
for dyn_call_list in dyn_call_lists:
    dyn_called_funcs = []
    with open(dyn_call_list) as f:
        for line in f:
            line = line.strip()
            dyn_called_funcs.append(line)
        num_glibc_funcs_called_by_spec.append(len(dyn_called_funcs))
        dfs([(func_name, 1) for func_name in dyn_called_funcs])











print()
print()

print('num_glibc_funcs_called_by_spec:   {}'.format(num_glibc_funcs_called_by_spec))
print("  max-min-avg: {}".format(min_max_avg(num_glibc_funcs_called_by_spec)))
print()

print('max_depths_of_static_call_chains: {}'.format(max_depths_of_static_call_chains))
print("  max-min-avg: {}".format(min_max_avg(max_depths_of_static_call_chains)))
print()

print('num_nodes_of_static_graphs:       {}'.format(num_nodes_of_static_graphs));
print("  max-min-avg: {}".format(min_max_avg(num_nodes_of_static_graphs)))
print()

print('num_edges_of_static_graphs:       {}'.format(num_edges_of_static_graphs));
print("  max-min-avg: {}".format(min_max_avg(num_edges_of_static_graphs)))
print()

print('num_non_divergent_functions:      {}'.format(num_non_divergent_functions));
print("  max-min-avg: {}".format(min_max_avg(num_non_divergent_functions)))
print()

print('num_key_errors:                   {}'.format(num_key_errors));
print("  max-min-avg: {}".format(min_max_avg(num_key_errors)))
print()
print()
print()
