#!/usr/bin/env python3.5
import subprocess

funcs = []
approved_list = []
return_codes = []

def write_approved_list():
    with open("approved_list.txt", "w") as f:
        for func in approved_list:
            f.write("%s\n" % func)

def run():
    #proc = subprocess.Popen(["./old_run_with_pin.sh"], stdout=subprocess.PIPE)
    #output = proc.communicate()
    rc = subprocess.call("./simple_run.sh", shell=True)
    return_codes.append(rc)
    return rc
    
def build():
    rc = subprocess.call("./simple_build.sh", shell=True)
    if rc:
        print("ERROR: simple_build.sh failed")
        exit(1)


# To get this script off the ground, useful to use a small list like
# called_funcs_in_susan.txt. To really run this, we need the glibc list.
#with open("called_funcs_in_susan.txt", "r") as f:
with open("../../libc/glibc_routine_list.txt", "r") as f:
    for line in f:
        funcs.append(line.strip())

build()

idx = 0
for func in funcs:
    approved_list.append(func)
    write_approved_list()
    rc = run()
    if rc != 0:
        approved_list.pop()
        print("drive_approved_list failed for %s" % func)
    idx += 1

print("drive_approved_list dumping return codes")
print(return_codes)
