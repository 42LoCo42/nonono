#!/bin/bash
rm groups
shopt -s nullglob
for g in /sys/kernel/iommu_groups/*; do
    echo "IOMMU Group ${g##*/}:" >> groups
    for d in $g/devices/*; do
        echo -e "\t$(lspci -nns ${d##*/})" >> groups
    done;
done;

cat groups
