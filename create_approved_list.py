#!/usr/bin/env python3.5
import subprocess
import sys
import os


funcs = []
approved_list = []
return_codes = []
ignored_list = set()


def write_list(list_name, base_path, list_filename):
    with open("%s/%s" % (base_path, list_filename), "w") as f:
        for func in list_name:
            f.write("%s\n" % func)

def run(benchmark, dataset):
    rc = subprocess.call("./drive_blankit.sh run %s %s blankit" % (benchmark, dataset), shell=True)
    return_codes.append(rc)
    return rc
    
def build():
    rc = subprocess.call("./drive_blankit.sh build", shell=True)
    if rc:
        print("ERROR: simple_build.sh failed")
        exit(1)

def usage():
    print()
    print("Usage:")
    print("  {} [bzip, gcc, ...] [test, train, ref] [path_to_benchmark] " \
          "[path_to_glibc_routine_list] [path_to_libm_routine_list] " \
          "[called_or_libname] [start_or_restart] " \
          "[restart_func_name]".format(sys.argv[0]))
    print()


# Parse args
if len(sys.argv) != 8 and len(sys.argv) != 9:
    usage()
    exit(1)

benchmark                  = sys.argv[1]
dataset                    = sys.argv[2]
path_to_benchmark          = sys.argv[3]
path_to_glibc_routine_list = sys.argv[4]
path_to_libm_routine_list  = sys.argv[5]
# How we gather the list of approved functions can be be
# "complete" (where we work from a full, 1743-function list of glibc functions)
# or not (where we just check the called funcs). This argument specifies
# how to create the list.
called_or_libname          = sys.argv[6]
start_or_restart           = sys.argv[7]
if len(sys.argv) == 9:
    restart_func_name      = sys.argv[8]

if "called" not in called_or_libname and "lib" not in called_or_libname:
   usage()
   exit(1)
if "start" not in start_or_restart and "restart" not in start_or_restart:
   usage()
   exit(1)



# Gather a list of functions for which we'll rerun our benchmark
if called_or_libname == "called":
    func_list_file = "%s/called_funcs_in_%s.txt" % (path_to_benchmark, benchmark)
elif called_or_libname == "libc":
    func_list_file = "%s/glibc_routine_list.txt" % path_to_glibc_routine_list
    approved_list_filename = "approved_list.txt"
    ignored_list_filename  = "ignored_list.txt"
elif called_or_libname == "libm":
    func_list_file = "%s/libm_routine_list.txt" % path_to_libm_routine_list
    approved_list_filename = "approved_list_libm.txt"
    ignored_list_filename  = "ignored_list_libm.txt"
else:
    usage()
    exit(1)

with open(func_list_file, "r") as f:
    for line in f:
        funcs.append(line.strip())



# Paranoid: Be sure our probe is compiled
build()


# hacky way to restart if profiling breaks in the middle
continuing = False
if start_or_restart == "restart":
    continuing = True
    prev_ignored = set()
    with open("%s/ignored_list.txt" % (path_to_benchmark)) as f:
        for line in f:
            prev_ignored.add(line.strip())


# Run the benchmark for every function, adding functions that don't crash to
# the approved list
idx = 0
for func in funcs:
    approved_list.append(func)
    if continuing: # if we were restarting, keep going til we reach that func
        if func == restart_func_name:
            continuing = False
        else:
            if func in prev_ignored:
                approved_list.pop()
                ignored_list.add(func)
            continue
    write_list(approved_list, path_to_benchmark, approved_list_filename)
    rc = run(benchmark, dataset)
    if rc != 0:
        approved_list.pop()
        ignored_list.add(func)
        print("drive_approved_list failed for %s" % func)
    idx += 1
print("drive_approved_list dumping return codes")
print(return_codes)



# If the last function didn't work, the approved-list-of-called-funcs file
# still has it and shouldn't. Write out one final copy to make sure it's
# correct
write_list(approved_list, path_to_benchmark, approved_list_filename)



# If we were testing with the full libc list, then our approved_list.txt is
# ready to go. Otherwise we back that file up and create the approved list
# from all funcs in glibc that didn't fail for the called funcs.
# XXX Support for libm does not exist for 'called'
if called_or_libname == "called":
    os.system("mv %s/approved_list.txt %s/approved_list_of_called_funcs.txt"
              % (path_to_benchmark, path_to_benchmark))
    with open("%s/approved_list.txt" % path_to_benchmark, "w") as f:
        with open("%s/glibc_routine_list.txt" % path_to_glibc_routine_list, "r") as ff:
            for func in ff:
                func = func.strip()
                if func not in ignored_list:
                    f.write("%s\n" % func)



# We printed the ignored funcs to stdout, but writing to file is much easier
# for checking later. And if we redirected to /dev/null and want to quickly
# check, this is easier than diff'ing approved list with glibc list.
write_list(ignored_list, path_to_benchmark, ignored_list_filename)
