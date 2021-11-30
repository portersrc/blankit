#!/bin/bash

# Calculate total gadgets over all libs
TOTAL_GADGETS=0
for f in `ls lib-rop-gadgets`; do
    GADGETS=$(wc -l lib-rop-gadgets/$f | awk '{print $1}')
    TOTAL_GADGETS=$((TOTAL_GADGETS + GADGETS))
done

# Count spurious gadgets outside text address
IGNORED_HIGH_ADDR=`./count_gadgets.py ignored_high_addr | awk '{sum+=$2} END{print sum}'`

# Substract out the spurious gadgets
TOTAL_GADGETS=$((TOTAL_GADGETS - IGNORED_HIGH_ADDR))

# Count max nginx gadgets
NGINX_GADGETS=`./count_gadgets.py count | awk '{print $2}'`

# Print reduction

printf "%% rop gadget reduction: "
echo "($TOTAL_GADGETS - $NGINX_GADGETS) / $TOTAL_GADGETS" | bc -l
