/**
* @file Experiments.h
* @brief Experimentos sobre autómatas celulares.
* @author Carlos Manuel Rodríguez Martínez
* @date 8/06/2015
*/

#ifndef _EXPERIMENTS
#define _EXPERIMENTS
#include "CellularAutomata.h"
#include "Export.h"


/*****************************
*                            *
* Parámetros de experimentos *
*                            *
*****************************/

/**
* @class ExParam
* @brief Parámetros de experimentos.
*/
class ExParam
{
public:
    CA_TYPE type;
    unsigned size, iterations, lanes;
    int vmax, vmax_min, vmax_max, init_vel;
    int random_seed, partitions;
    int porder, pinterval;
    int threads;

    double density, density_min, density_max;
    double rand_prob, rand_prob_min, rand_prob_max;
    double aut_car_density_min, aut_car_density_max;
    double new_car_prob_min, new_car_prob_max;
    double stop_density_min, stop_density_max;
    double semaphore_density_min, semaphore_density_max;
    double dt;

    bool per_density, per_prob, random_semaphores, show_progress;

    std::string path, out_file_name;

    Args args;

    ExportFormat export_format;

    void Report() const;
    std::string GetFilePath(std::string filename) const;
};


/****************************
*                           *
*        Experimentos       *
*                           *
****************************/

/**
* @brief Crea mapa de tráfico.
*/
void ex_traffic_map(ExParam p);

/**
* @brief Crea mapa de flujo.
*/
void ex_flow_map(ExParam p);

/**
* @brief Mide la ocupación en todas las casillas de AC.
*/
void ex_ocupancy_fixed(ExParam p);

/**
* @brief Mide el flujo en todas las casillas de AC.
*/
void ex_flow_fixed(ExParam p);

/**
* @brief Mide el flujo promedio de todas las casillas respecto a densidad de autos.
*/
void ex_flow_vs_density(ExParam p);

/**
* @brief Mide el flujo promedio de todas las casillas respecto a velocidad máxima autos.
*/
void ex_flow_vs_vmax(ExParam p);

/**
* @brief Mide el flujo promedio de todas las casillas respecto a velocidad máxima autos.
*/
void ex_flow_vs_rand_prob(ExParam p);

/**
* @brief Mide el flujo promedio de todas las casillas respecto a densidad de autos inteligentes.
*/
void ex_flow_vs_aut_cars(ExParam p);

/**
* @brief Mide el flujo promedio de todas las casillas respecto a probabilidad de aparición de nuevo auto.
*/
void ex_flow_vs_new_car_prob(ExParam p);

/**
* @brief Mide el flujo promedio de todas las casillas respecto a densidad de topes.
*/
void ex_flow_vs_stop_density(ExParam p);

/**
* @brief Mide el flujo promedio de todas las casillas respecto a densidad de semáforos.
*/
void ex_flow_vs_semaphore_density(ExParam p);

/**
* @brief Mide el tiempo de escape respecto a densidad de vehículos.
*/
void ex_escape_time_vs_density(ExParam p);

/**
* @brief Mide el tiempo de escape respecto a probabilidad de descenso de velocidad.
*/
void ex_escape_time_vs_rand_prob(ExParam p);

/**
* @brief Mide el tiempo de escape respecto a velocidad máxima.
*/
void ex_escape_time_vs_vmax(ExParam p);

/**
* @brief Mide el gasto respecto a densidad de vehículos.
*/
void ex_discharge_vs_density(ExParam p);

/**
* @brief Crea gráfica de entropia de permutacion de gasto respecto a densidad de vehículos.
*/
void ex_pentropy_vs_density(ExParam p);

/**
* @brief Mide el número de cambios de carril respecto a densidad.
*/
void ex_lane_changes_vs_density(ExParam p);


/**
* @brief Realiza tests para verificar la corrección de los algoritmos.
*/
void ex_perform_test();

#endif