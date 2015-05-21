#!/bin/bash

for P in 100 1000 2000 3000 4000 5000 6000 7000 8000 9000
do
   ../Release/Freeway --size=$P --iter=1000 --rand_prob=0.2 --vmax=5 --flow_vs_density --dmin=0.0 --dmax=1.0 --dt=0.005 --out_file_name="flow_vs_density_$P.csv" > /dev/null &
done
wait