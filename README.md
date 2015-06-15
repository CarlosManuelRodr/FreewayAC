# FreewayAC
Implementación de simulación de tráfico usando un autómata celular.

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

# Requisitos
Requiere un compilador de C++ con soporte para el estándar C++11.