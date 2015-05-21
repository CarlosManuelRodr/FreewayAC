#!/bin/bash

for P in 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9
do
   ../Release/Freeway --size=1000 --iter=1000 --rand_prob=$P --flow_vs_density --dmin=0.0 --dmax=1.0 --dt=0.005 --out_file_name="flow_vs_density_$P.csv" > /dev/null &
done
wait