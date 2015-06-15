# FreewayAC
Implementación de simulación de tráfico usando un autómata celular.

![CA](Resultados/Circular/ca.png?raw=true "CA") 

# Instrucciones
Ejemplo de uso:
"FreewayAC --size=1000 --iter=1000 --density=0.2 --rand_prob=0.3 --ca_circular --flow_vs_density --dmin=0.0 --dmax=1.0 --dt=0.001"

```
Opciones:
  -s <arg>, --size=<arg>                     
                    Tamagno del AC.
  -i <arg>, --iter=<arg>                     
                    Iteraciones del AC.
  -v <arg>, --vmax=<arg>                     
                    Velocidad maxima de auto.
  -d <arg>, --density=<arg>                  
                    Densidad de autos.
  -r <arg>, --rand_prob=<arg>                
                    Probabilidad de descenso de velocidad.
  -i <arg>, --init_vel=<arg>                 
                    Velocidad inicial de los autos.
  -p ,      --plot_traffic                   
                    Crea mapa de posicion de autos vs tiempo.
  -p ,      --plot_flow                      
                    Crea mapa de flujo de autos vs tiempo.
  -o ,      --ocupancy_fixed                 
                    Mide la densidad de ocupacion promedio de cada casilla.
  -f ,      --flow_fixed                     
                    Mide el flujo promedio de cada casilla.
            --flow_vs_density                
                    Mide el flujo respecto a densidad en un rango especificado
                    por dmin, dmax y dt.
            --flow_per_density               
                    Mide el flujo por densidad respecto a densidad en un rango
                    especificado por dmin, dmax y dt.
            --flow_vs_vmax                   
                    Mide el flujo respecto a vmax en un rango especificado por
                    vmax_min, vmax_max y dt.
            --flow_vs_rand_prob              
                    Mide el flujo respecto a probabilidad de frenado en un rango
                    especificado por rand_min, rand_max y dt.
            --flow_vs_aut_cars               
                    Mide el flujo respecto a densidad de vehiculos autonomos en
                    un rango especificado por aut_min, aut_max y dt.
            --flow_vs_stop_density           
                    Mide el flujo respecto a densidad de topes en un rango
                    especificado por stop_density_min, stop_density_max y dt.
            --flow_vs_new_car                
                    Flujo respecto a probabilidad de nuevo auto en ac abierto en
                    un rango especificado por new_car_min, new_car_max y dt.
            --flow_per_new_car               
                    Flujo/probabilidad respecto a probabilidad de nuevo auto en
                    ac abierto.
            --flow_vs_semaphore_density      
                    Flujo respecto a densidad de semaforos en un rango
                    especificado por semaphore_density_min y
                    semaphore_density_max.
            --escape_time_vs_density         
                    Mide tiempo de escape vs densidad en un rango especificado
                    por dmin, dmax y dt.
            --discharge_vs_density           
                    Mide gasto vs densidad en un rango especificado por dmin,
                    dmax y dt.
            --discharge_vs_density_fractal   
                    Crea fractal de gasto vs densidad en un rango especificado
                    por dmin, dmax y dt.
            --dimension_vs_density           
                    Mide dimension fractal vs densidad en un rango especificado
                    por dmin, dmax y dt.
            --dimension_vs_density_threaded  
                    Mide dimension fractal vs densidad usando varios núcleos en
                    un rango especificado por dmin, dmax y dt.
            --ca_circular                    
                    Automata celular circular.
            --ca_multilane_circular          
                    Automata celular multicarril circular.
            --ca_open                        
                    Automata celular con frontera abierta.
            --ca_multilane_open              
                    Automata celular multicarril con frontera abierta.
            --ca_autonomous                  
                    Automata celular con vehiculos autonomos.
            --ca_stop                        
                    Automata celular con tope. La posicion del tope se
                    especifica por stop_density.
            --ca_semaphore                   
                    Automata celular con semaforo. La posicion del semaforo se
                    especifica por semaphore_density.
            --ca_simple_junction             
                    Automata de interseccion simple.
            --new_car_prob                   
                    Probabilidad de que se aparezca nuevo auto en frontera
                    abierta.
            --new_car_speed                  
                    Velocidad que entre a AC abierto.
            --aut_density                    
                    Densidad de autos autonomos.
            --stop_density=<arg>             
                    Densidad de topes en ca_stop.
            --semaphore_density=<arg>        
                    Densidad de semaforos en ca_semaphore.
            --random_semaphores              
                    True o False. Coloca los semaforos en posiciones aleatorias
                    en vez de uniformes.
            --target_lane                    
                    Carril objetivo en AC de uniones.
            --lanes                          
                    Numero de carriles en AC multicarril.
            --random_seed                    
                    Semilla de generador de numeros aleatorios. Si no se
                    especifica usa el tiempo del sistema.
            --random_generator               
                    Tipo de generador de aletorios. Disponibles LCG, MT19937,
                    RANLUX24, RANLUX48.
            --partitions                     
                    Numero de particiones que se realizan sobre un intervalo
                    para calcular dimension fractal.
            --dt=<arg>                       
                    Tamagno del intervalo en medicion con rango.
            --dmin=<arg>                     
                    Densidad minima en medicion con rango.
            --dmax=<arg>                     
                    Densidad maxima en medicion con rango.
            --vmax_min=<arg>                 
                    Vmax minima en medicion con rango.
            --vmax_max=<arg>                 
                    Vmax maxima en medicion con rango.
            --rand_prob_min=<arg>            
                    Probabilidad de frenado minima en medicion con rango.
            --rand_prob_max=<arg>            
                    Probabilidad de frenado maxima en medicion con rango.
            --aut_min=<arg>                  
                    Densidad minima de autos autonomos.
            --aut_max=<arg>                  
                    Densidad maxima de autos autonomos.
            --stop_density_min=<arg>         
                    Densidad minima de topes.
            --stop_density_max=<arg>         
                    Densidad maxima de topes.
            --semaphore_density_min=<arg>    
                    Densidad minima de semaforos.
            --semaphore_density_max=<arg>    
                    Densidad maxima de semaforos.
            --new_car_min=<arg>              
                    Probabilidad minima de nuevo auto en ac abierto.
            --new_car_max=<arg>              
                    Probabilidad maxima de nuevo auto en ac abierto.
            --out_file_name=<arg>            
                    Cambia el nombre del archivo de salida al especificado.
            --path=<arg>                     
                    Ruta donde guardar archivos de salida.
            --export_format=<arg>            
                    Formato de salida. CSV o BMP.
            --show_progress=<arg>            
                    True o False. Muestra o no barra de progreso.
            --test                           
                    Realiza pruebas para garantizar la fiabilidad de resultados.
            --debug                          
                    Modo de depuracion.
            --help                           
                    Muestra instrucciones detalladas de cada experimento.

```

# Tipos de autómata celular
```
CA_CIRCULAR           -> Descripcion: Automata celular con fronteras periodicas. Pista circular.
                         Parametros relevantes: Ninguno.
CA_MULTILANE_CIRCULAR -> Descripcion: Automata celular multicarril con fronteras periodicas.
                         Parametros relevantes: LANES.
CA_OPEN               -> Descripcion: Automata celular con fronteras abiertas. Entran autos en
                                      la primera pos del AC.
CA_MULTILANE_OPEN     -> Descripcion: Automata celular con fronteras abiertas. Entran autos en
                                      la primera pos del AC.
                         Parametros relevantes: NEW_CAR_PROB, NEW_CAR_SPEED, LANES.
CA_AUTONOMOUS         -> Descripcion: Automata celular circular con vehiculos autonomos.
                         Parametros relevantes: AUT_DENSITY.
CA_STOP               -> Descripcion: Automata celular circular con topes en la pista.
                         Parametros relevantes: STOP_DENSITY.
CA_SEMAPHORE          -> Descripcion: Automata celular circular con semaforos en la pista.
                         Parametros relevantes: SEMAPHORE_DENSITY, RANDOM_SEMAPHORES.
CA_SIMPLE_JUNCTION    -> Descripcion: Automata celular abierto con unión simple y fronteras abiertas.
                         Parametros relevantes: NEW_CAR_PROB, NEW_CAR_SPEED, TARGET_LANE.
```

# Tipos de experimento
```
PLOT_TRAFFIC          -> Descripcion: Evoluciona automata celular y grafica su representacion.
                         Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL.
PLOT_FLOW             -> Descripcion: Evoluciona automata celular y grafica su flujo.
                         Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL.
MEASURE_OCUPANCY      -> Descripcion: Mide la ocupacion de cada casilla del automata celular.
                         Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL.
MEASURE_FLOW          -> Descripcion: Mide el flujo de cada casilla del automata celular.
                         Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL.

FLOW_VS_DENSITY       -> Descripcion: Mide el flujo de autos promedio respecto a los valores
                                      de densidad de autos especificados en un rango.
                         Parametros relevantes: SIZE, ITERATIONS, VMAX, RAND_PROB, INIT_VEL, DMIN, DMAX, DT.
FLOW_PER_DENSITY      -> Descripcion: Mide el flujo de autos promedio sobre densidad respecto
                                      a los valores de densidad de autos especificados en un rango.
                         Parametros relevantes: SIZE, ITERATIONS, VMAX, RAND_PROB, INIT_VEL, DMIN, DMAX, DT.
FLOW_VS_VMAX          -> Descripcion: Mide el flujo de autos promedio respecto a los valores de
                                      velocidad maxima especificados en un rango.
                         Parametros relevantes: SIZE, ITERATIONS, DENSITY, RAND_PROB, INIT_VEL, VMAX_MIN, VMAX_MAX, DT.
FLOW_VS_RAND_PROB     -> Descripcion: Mide el flujo de autos promedio respecto a los valores de
                                      probabilidad de descenso de velocidad especificados en un rango.
                         Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, INIT_VEL, RAND_PROB_MIN,
                                                RAND_PROB_MAX, DT.
FLOW_VS_AUT_CARS      -> Descripcion: Mide el flujo de autos promedio respecto a los valores
                                      de densidad de vehiculos autonomos especificados en un rango.
                         Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL, 
                                                AUT_MIN, AUT_MAX, DT.
FLOW_VS_STOP_DENSITY  -> Descripcion: Mide el flujo de autos promedio respecto a los valores
                                      densidad de topes especificados en un rango. Se usa el
                                      automata celular CA_STOP.
                         Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL,
                                                STOP_DENSITY_MIN, STOP_DENSITY_MAX, DT.
FLOW_VS_NEW_CAR       -> Descripcion: Mide el flujo de autos promedio respecto a los valores
                                      de probabilidad de aparicion de nuevo auto especificados
                                      en un rango. Se usa el automata celular CA_OPEN o.
                                      CA_SIMPLE_JUNCTION.
                         Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL,
                                                NEW_CAR_MIN, NEW_CAR_MAX, DT.
FLOW_PER_NEW_CAR      -> Descripcion: Mide el flujo/prob de autos promedio respecto a los valores
                                      de probabilidad de aparicion de nuevo auto especificados
                                      en un rango. Se usa el automata celular CA_OPEN.
                         Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL,
                                                NEW_CAR_MIN, NEW_CAR_MAX, DT.
FLOW_VS_SEMAPHORE_DENSITY  -> Descripcion: Mide el flujo de autos promedio respecto a los valores de
                              densidad de semaforos especificados en un rango. Se usa el automata
                              celular ca_semaphore.
                              Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL,
                                                     SEMAPHORE_DENSITY_MIN, SEMAPHORE_DENSITY_MIN,
                                                     RANDOM_SEMAPHORES, DT.
ESCAPE_TIME_VS_DENSITY     -> Descripcion: Mide el tiempo de escape respecto a los valores de
                              densidad de vehiculos especificados en un rango. Se usa el automata
                              celular ca_open.
                              Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL,
                                                     DMIN, DMAX, DT.
DISCHARGE_VS_DENSITY       -> Descripcion: Mide el gasto respecto a los valores de
                              densidad de vehiculos especificados en un rango. Se usa el automata
                              celular ca_open.
                              Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL,
                                                     DMIN, DMAX, DT.
DISCHARGE_VS_DENSITY_FRACTAL -> Descripcion: Mide la dimension fractal en un mapa creado por los valores de
                                densidad de vehiculos especificados en un rango. Se usa el automata
                                celular ca_open.
                                Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL,
                                                       DMIN, DMAX, DT.
DIMENSION_VS_DENSITY      -> Descripcion: Crea grafica de dimension fractal respecto a los valores de
                              densidad de vehículos especificados en un rango. Se usa el automata
                              celular ca_open.
                              Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL,
                                                     DMIN, DMAX, DT, PARTITIONS.
```

# Requisitos
Requiere un compilador de C++ con soporte para el estándar C++11.