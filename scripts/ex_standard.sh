#!/bin/bash

# Valores opcionales por defecto.
SIZE=1000
ITER=1000
DENSITY=0.2
RAND_PROB=0.3
NEW_CAR_PROB=0.4
NEW_CAR_SPEED=2

AUT_DENSITY=0.2
STOP_DENSITY=0.1
SEMAPHORE_DENSITY=0.1
RANDOM_SEMAPHORES="False"
TARGET_LANE=1
LANES=2
DT=0.005

# Lee parametros.
TEMP=`getopt -o a:b:c:d:e:f:g:h:i:j:k:l:m: --long size:,iter:,density:,rand_prob:,new_car_prob:,new_car_speed:\
,aut_density:,stop_density:,semaphore_density:,random_semaphores:,target_lane:,lanes:,dt: -n 'ex_standard.sh' -- "$@"`
eval set -- "$TEMP"

# Extrae parametros.
while true ; do
    case "$1" in
        -a|--size)
            case "$2" in
                "") shift 2 ;;
                *) SIZE=$2 ; shift 2 ;;
            esac ;;
        -b|--iter)
            case "$2" in
                "") shift 2 ;;
                *) ITER=$2 ; shift 2 ;;
            esac ;;
        -c|--density)
            case "$2" in
                "") shift 2 ;;
                *) DENSITY=$2 ; shift 2 ;;
            esac ;;
        -d|--rand_prob)
            case "$2" in
                "") shift 2 ;;
                *) RAND_PROB=$2 ; shift 2 ;;
            esac ;;
        -e|--new_car_prob)
            case "$2" in
                "") shift 2 ;;
                *) NEW_CAR_PROB=$2 ; shift 2 ;;
            esac ;;
        -f|--new_car_speed)
            case "$2" in
                "") shift 2 ;;
                *) NEW_CAR_SPEED=$2 ; shift 2 ;;
            esac ;;
        -g|--aut_density)
            case "$2" in
                "") shift 2 ;;
                *) AUT_DENSITY=$2 ; shift 2 ;;
            esac ;;
        -h|--stop_density)
            case "$2" in
                "") shift 2 ;;
                *) STOP_DENSITY=$2 ; shift 2 ;;
            esac ;;
        -i|--semaphore_density)
            case "$2" in
                "") shift 2 ;;
                *) SEMAPHORE_DENSITY=$2 ; shift 2 ;;
            esac ;;
        -j|--random_semaphores)
            case "$2" in
                "") shift 2 ;;
                *) RANDOM_SEMAPHORES=$2 ; shift 2 ;;
            esac ;;
        -k|--target_lane)
            case "$2" in
                "") shift 2 ;;
                *) TARGET_LANE=$2 ; shift 2 ;;
            esac ;;
        -l|--lanes)
            case "$2" in
                "") shift 2 ;;
                *) LANES=$2 ; shift 2 ;;
            esac ;;
        -m|--dt)
            case "$2" in
                "") shift 2 ;;
                *) DT=$2 ; shift 2 ;;
            esac ;;
        --) shift ; break ;;
        *) echo "Internal error!" ; exit 1 ;;
    esac
done

echo "Ejecutando simulaciones con los siguientes parametros."
echo "size: $SIZE"
echo "iter: $ITER"
echo "density: $DENSITY"
echo "rand_prob: $RAND_PROB"
echo "new_car_prob: $NEW_CAR_PROB"
echo "new_car_speed: $NEW_CAR_SPEED"
echo "aut_density: $AUT_DENSITY"
echo "stop_density: $STOP_DENSITY"
echo "semaphore_density: $SEMAPHORE_DENSITY"
echo "random_semaphores: $RANDOM_SEMAPHORES"
echo "target_lane: $TARGET_LANE"
echo "lanes: $LANES"
echo "dt: $DT"

# Circular
echo "Circular CA"
rm -rf "Circular"
mkdir "Circular"
cp "plot.m" "Circular/"
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --rand_prob=$RAND_PROB --ca_circular --path="Circular/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --rand_prob=$RAND_PROB --ca_circular --flow_vs_density --dmin=0.0 --dmax=1.0 --dt=$DT \
                   --path="Circular/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --rand_prob=$RAND_PROB --ca_circular --flow_per_density --dmin=0.0 --dmax=1.0 --dt=$DT \
                   --path="Circular/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --rand_prob=$RAND_PROB --ca_circular --flow_vs_vmax --vmax_min=1 \
                   --vmax_max=15 --dt=1 --path="Circular/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --ca_circular --flow_vs_rand_prob --rand_prob_min=0.0 --rand_prob_max=1.0 \
                   --dt=$DT --path="Circular/" --show_progress=false &

wait

# Open
echo "Open CA"
rm -rf "Open"
mkdir "Open"
cp "plot.m" "Open/"
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --rand_prob=$RAND_PROB --ca_open --new_car_prob=$NEW_CAR_PROB \
                   --new_car_speed=$NEW_CAR_SPEED --path="Open/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --rand_prob=$RAND_PROB --ca_open --new_car_prob=$NEW_CAR_PROB --new_car_speed=$NEW_CAR_SPEED \
                   --flow_vs_density --dmin=0.0 --dmax=1.0 --dt=$DT --path="Open/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --rand_prob=$RAND_PROB --ca_open --new_car_prob=$NEW_CAR_PROB --new_car_speed=$NEW_CAR_SPEED \
                   --flow_per_density --dmin=0.0 --dmax=1.0 --dt=$DT --path="Open/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --rand_prob=$RAND_PROB --ca_open --new_car_prob=$NEW_CAR_PROB \
                   --new_car_speed=$NEW_CAR_SPEED --flow_vs_vmax --vmax_min=1 --vmax_max=15 --dt=1 --path="Open/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --ca_open --new_car_prob=$NEW_CAR_PROB --new_car_speed=$NEW_CAR_SPEED \
                   --flow_vs_rand_prob --rand_prob_min=0.0 --rand_prob_max=1.0 --dt=$DT --path="Open/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --ca_open --new_car_speed=$NEW_CAR_SPEED --flow_vs_new_car \
                   --new_car_min=0.0 --new_car_max=1.0 --dt=$DT --path="Open/" --show_progress=false &

wait

echo "Autonomous CA"
rm -rf "Autonomous"
mkdir "Autonomous"
cp "plot.m" "Autonomous/"
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --rand_prob=$RAND_PROB --ca_autonomous --aut_density=$AUT_DENSITY \
                   --path="Autonomous/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --rand_prob=$RAND_PROB --ca_autonomous --aut_density=$AUT_DENSITY  \
                   --flow_vs_density --dmin=0.0 --dmax=1.0 --dt=$DT --path="Autonomous/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --rand_prob=$RAND_PROB --ca_autonomous --aut_density=$AUT_DENSITY  \
                   --flow_per_density --dmin=0.0 --dmax=1.0 --dt=$DT --path="Autonomous/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --rand_prob=$RAND_PROB --ca_autonomous --aut_density=$AUT_DENSITY \
                    --flow_vs_vmax --vmax_min=1 --vmax_max=15 --dt=1 --path="Autonomous/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --ca_autonomous --aut_density=$AUT_DENSITY  \
                   --flow_vs_rand_prob --rand_prob_min=0.0 --rand_prob_max=1.0 --dt=$DT --path="Autonomous/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --ca_autonomous --flow_vs_aut_cars --aut_min=0.0 \
                   --aut_max=1.0 --dt=$DT --path="Autonomous/" --show_progress=false &

wait

# Stop
echo "Stop CA"
rm -rf "Stop"
mkdir "Stop"
cp "plot.m" "Stop/"
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --rand_prob=$RAND_PROB --ca_stop --stop_density=$STOP_DENSITY \
                   --path="Stop/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --rand_prob=$RAND_PROB --ca_stop --stop_density=$STOP_DENSITY  \
                   --flow_vs_density --dmin=0.0 --dmax=1.0 --dt=$DT --path="Stop/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --rand_prob=$RAND_PROB --ca_stop --stop_density=$STOP_DENSITY  \
                   --flow_per_density --dmin=0.0 --dmax=1.0 --dt=$DT --path="Stop/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --rand_prob=$RAND_PROB --ca_stop --stop_density=$STOP_DENSITY \
                   --flow_vs_vmax --vmax_min=1 --vmax_max=15 --dt=1 --path="Stop/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --ca_stop --stop_density=$STOP_DENSITY  \
                   --flow_vs_rand_prob --rand_prob_min=0.0 --rand_prob_max=1.0 --dt=$DT --path="Stop/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --ca_stop  --flow_vs_stop_density \
                   --stop_density_min=0.0 --stop_density_max=1.0 --dt=$DT --path="Stop/" --show_progress=false &

wait

# Semaphore
echo "Semaphore CA"
rm -rf "Semaphore"
mkdir "Semaphore"
cp "plot.m" "Semaphore/"
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --rand_prob=$RAND_PROB --ca_semaphore --semaphore_density=$SEMAPHORE_DENSITY \
                   --random_semaphores=$RANDOM_SEMAPHORES --path="Semaphore/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --rand_prob=$RAND_PROB --ca_semaphore --semaphore_density=$SEMAPHORE_DENSITY  \
                   --random_semaphores=$RANDOM_SEMAPHORES --flow_vs_density --dmin=0.0 --dmax=1.0 --dt=$DT --path="Semaphore/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --rand_prob=$RAND_PROB --ca_semaphore --semaphore_density=$SEMAPHORE_DENSITY  \
                   --random_semaphores=$RANDOM_SEMAPHORES --flow_per_density --dmin=0.0 --dmax=1.0 --dt=$DT --path="Semaphore/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --rand_prob=$RAND_PROB --ca_semaphore --semaphore_density=$SEMAPHORE_DENSITY \
                   --random_semaphores=$RANDOM_SEMAPHORES --flow_vs_vmax --vmax_min=1 --vmax_max=15 --dt=1 --path="Semaphore/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --ca_semaphore --semaphore_density=$SEMAPHORE_DENSITY  \
                   --random_semaphores=$RANDOM_SEMAPHORES --flow_vs_rand_prob --rand_prob_min=0.0 --rand_prob_max=1.0 \
                   --dt=$DT --path="Semaphore/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --ca_semaphore  --flow_vs_semaphore_density \
                   --random_semaphores=$RANDOM_SEMAPHORES --semaphore_density_min=0.0 --semaphore_density_max=1.0 \
                   --dt=$DT --path="Semaphore/" --show_progress=false &

wait

# SimpleJunction
echo "SimpleJunction CA"
rm -rf "SimpleJunction"
mkdir "SimpleJunction"
cp "plot.m" "SimpleJunction/"
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --rand_prob=$RAND_PROB --ca_simple_junction --new_car_prob=$NEW_CAR_PROB \
                   --new_car_speed=$NEW_CAR_SPEED --path="SimpleJunction/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --rand_prob=$RAND_PROB --ca_simple_junction --new_car_prob=$NEW_CAR_PROB --new_car_speed=$NEW_CAR_SPEED \
                   --flow_vs_density --dmin=0.0 --dmax=1.0 --dt=$DT --path="SimpleJunction/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --rand_prob=$RAND_PROB --ca_simple_junction --new_car_prob=$NEW_CAR_PROB --new_car_speed=$NEW_CAR_SPEED \
                   --flow_per_density --dmin=0.0 --dmax=1.0 --dt=$DT --path="SimpleJunction/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --rand_prob=$RAND_PROB --ca_simple_junction --new_car_prob=$NEW_CAR_PROB \
                   --new_car_speed=$NEW_CAR_SPEED --flow_vs_vmax --vmax_min=1 --vmax_max=15 --dt=1 --path="SimpleJunction/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --ca_simple_junction --new_car_prob=$NEW_CAR_PROB --new_car_speed=$NEW_CAR_SPEED \
                   --flow_vs_rand_prob --rand_prob_min=0.0 --rand_prob_max=1.0 --dt=$DT --path="SimpleJunction/" --show_progress=false &
../bin/Freeway --size=$SIZE --iter=$ITER --density=$DENSITY --ca_simple_junction --new_car_speed=$NEW_CAR_SPEED --flow_vs_new_car \
                   --new_car_min=0.0 --new_car_max=1.0 --dt=$DT --path="SimpleJunction/" --show_progress=false &

wait

# Circular
echo "Multilane Circular CA"
rm -rf "MultilaneCircular"
mkdir "MultilaneCircular"
cp "plot.m" "MultilaneCircular/"
../bin/Freeway --size=$SIZE --lanes=$LANES --iter=$ITER --density=$DENSITY --rand_prob=$RAND_PROB --ca_multilane_circular \
                   --path="MultilaneCircular/" --show_progress=false &
../bin/Freeway --size=$SIZE --lanes=$LANES --iter=$ITER --rand_prob=$RAND_PROB --ca_multilane_circular --flow_vs_density \
                   --dmin=0.0 --dmax=1.0 --dt=$DT --path="MultilaneCircular/" --show_progress=false &
../bin/Freeway --size=$SIZE --lanes=$LANES --iter=$ITER --rand_prob=$RAND_PROB --ca_multilane_circular --flow_per_density \
                   --dmin=0.0 --dmax=1.0 --dt=$DT --path="MultilaneCircular/" --show_progress=false &

wait

math -script "Circular/plot.m"
math -script "Open/plot.m"
math -script "Autonomous/plot.m"
math -script "Stop/plot.m"
math -script "Semaphore/plot.m"
math -script "SimpleJunction/plot.m"
math -script "MultilaneCircular/plot.m"