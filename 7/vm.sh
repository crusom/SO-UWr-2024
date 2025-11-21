#!/bin/bash

vmsize_sum=0
vmrss_sum=0
for pid in $(ps -e | awk '{print $1}'); do
    f="/proc/$pid/status"
    if [ ! -f $f  ]; then continue; fi
    vmsize=$(grep -i "VmSize" $f | awk '{print $2}')
    vmrss=$(grep -i "VmRSS" $f | awk '{print $2}')
    vmsize_sum=$((vmsize_sum + vmsize))
    vmrss_sum=$((vmrss_sum + vmrss))
done

echo "Suma VmSize: $vmsize_sum kB"
echo "Suma VmRSS: $vmrss_sum kB"
