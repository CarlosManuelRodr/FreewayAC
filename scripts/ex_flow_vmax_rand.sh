#!/bin/bash

for P in 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9
do
   ../bin/Freeway --size=5000 --iter=5000 --rand_prob=$P --density=0.2 --flow_vs_vmax --vmax_min=0 --vmax_max=15 --dt=1 --out_file_name="flow_vs_vmax_$P.csv" > /dev/null &
done
wait