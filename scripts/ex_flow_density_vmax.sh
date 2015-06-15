#!/bin/bash

for P in 1 2 3 4 5 6 7 8 9
do
   ../bin/Freeway --size=1000 --iter=1000 --rand_prob=0.2 --vmax=$P --flow_vs_density --dmin=0.0 --dmax=1.0 --dt=0.005 --out_file_name="flow_vs_density_$P.csv" > /dev/null &
done
wait