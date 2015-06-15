#!/bin/bash

for P in 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9
do
   ../bin/Freeway --size=1000 --iter=1000 --rand_prob=$P --density=0.2 --flow_vs_smart_cars --smart_min=0.0 --smart_max=1.0 --dt=0.01 --out_file_name="flow_vs_smart_$P.csv" > /dev/null &
done
wait