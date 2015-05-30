#!/bin/bash

# Circular
mkdir "Circular"
../Release/Freeway --size=1000 --iter=1000 --rand_prob=0.3 --ca_circular --flow_vs_density --dmin=0.0 --dmax=1.0 --dt=0.005 --out_file_name="Circular/flow_vs_density.csv" > /dev/null &
../Release/Freeway --size=1000 --iter=1000 --rand_prob=0.3 --ca_circular --flow_per_density --dmin=0.0 --dmax=1.0 --dt=0.005 --out_file_name="Circular/flow_per_density.csv" > /dev/null &
../Release/Freeway --size=1000 --iter=1000 --rand_prob=0.3 --ca_circular --flow_vs_vmax --vmax_min=0 --vmax_max=15 --dt=1 --out_file_name="Circular/flow_vs_vmax.csv" > /dev/null &
../Release/Freeway --size=1000 --iter=1000 --rand_prob=0.3 --ca_circular --flow_vs_rand_prob --rand_prob_min=0.0 --rand_prob_max=15 --dt=0.005 --out_file_name="Circular/flow_vs_rand_prob.csv" > /dev/null &
wait