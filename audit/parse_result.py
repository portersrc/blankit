#!/usr/bin/env python3.5


BASE_FOLDER = "audit_results"
#BASE_FOLDER = "auditmem_results"

result_files = [
    "astar_results",
    "gobmk_results",
    "lbm_results",
    "milc_results",
    "povray_results",
    "sphinx_results",
    "bzip_results",
    "h264ref_results",
    "libquantum_results",
    "namd_results",
    "sjeng_results",
    "xalancbmk_results",
    "gcc_results",
    "hmmer_results",
    "mcf_results",
    "omnetpp_results",
    "soplex_results",
]

class CountSum:
    def __init__(self, c, s):
        self.c = c
        self.s = s
    

for result_file in result_files:
    print("Beginning parse for %s" % result_file)
    func_to_count_sum = {}
    with open("%s/%s" % (BASE_FOLDER, result_file)) as f:
        for line in f:
            line = line.strip()
            if line == "":
                continue
            line_vec = line.split()
            if line_vec[0] == "AUDIT_NANOSECONDS":
                try:
                    func_name = line_vec[1]
                    exec_time = int(line_vec[2])
                except:
                    continue
                
                if func_name not in func_to_count_sum:
                    func_to_count_sum[func_name] = CountSum(0, 0)
                func_to_count_sum[func_name].c += 1
                func_to_count_sum[func_name].s += exec_time

    with open("%s/%s.avg" % (BASE_FOLDER, result_file), "w") as f:
        for func_name, count_sum in func_to_count_sum.items():
            avg = 1.0 * count_sum.s / count_sum.c
            f.write("%s %f\n" % (func_name, avg))
