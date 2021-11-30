#!/usr/bin/env python3
import sys
import os


#
# copied from original spec rop-gadget folder. see more details there.
#



total = 0.0
count = 0

base_folder = "quality"

files = os.listdir(base_folder)
for filename in files:
    full_name = base_folder + '/' + filename
    #with open(sys.argv[1]) as f:
    with open(full_name) as f:
        for line in f:
            line = line.strip()
            if "Average gadget score" in line:
                line_vec = line.split(':')
                score = line_vec[1].strip()
                if score == "NaN":
                    # we could do something here... im not sure i trust these
                    # cases enough, though, to claim that we had 0 gadgets in the
                    # call chain
                    pass
                else:
                    score = float(score)
                    total += score
                    count += 1
avg = total / count
print("nginx %f" % (avg))



