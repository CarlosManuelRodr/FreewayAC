#pragma once
#ifndef _EXPERIMENTS
#define _EXPERIMENTS

#include "CellularAutomata.h"


/*****************************
*                            *
* Parámetros de experimentos *
*                            *
*****************************/

struct ExParam
{
    CA_TYPE type;
    unsigned size, iterations, lanes;
    int vmax, vmax_min, vmax_max;
    int random_seed, partitions;

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
};


/****************************
*                           *
*          Medición         *
*                           *
****************************/

double measure_fractal_dimension(std::vector<int> frac);


/****************************
*                           *
*        Experimentos       *
*                           *
****************************/

int ex_traffic_maps(ExParam p);

int ex_flow_maps(ExParam p);

/**
* @brief Mide la ocupación en todas las casillas de AC.
*/
int ex_ocupancy_fixed(ExParam p);

/**
* @brief Mide el flujo en todas las casillas de AC.
*/
int ex_flow_fixed(ExParam p);

/**
* @brief Mide el flujo promedio de todas las casillas respecto a densidad de autos.
*/
int ex_flow_vs_density(ExParam p);

/**
* @brief Mide el flujo promedio de todas las casillas respecto a densidad de autos.
*/
int ex_multilane_flow_vs_density(ExParam p);

/**
* @brief Mide el flujo promedio de todas las casillas respecto a velocidad máxima autos.
*/
int ex_flow_vs_vmax(ExParam p);

/**
* @brief Mide el flujo promedio de todas las casillas respecto a velocidad máxima autos.
*/
int ex_flow_vs_rand_prob(ExParam p);

/**
* @brief Mide el flujo promedio de todas las casillas respecto a densidad de autos inteligentes.
*/
int ex_flow_vs_aut_cars(ExParam p);

/**
* @brief Mide el flujo promedio de todas las casillas respecto a probabilidad de aparición de nuevo auto.
*/
int ex_flow_vs_new_car_prob(ExParam p);

/**
* @brief Mide el flujo promedio de todas las casillas respecto a densidad de topes.
*/
int ex_flow_vs_stop_density(ExParam p);

/**
* @brief Mide el flujo promedio de todas las casillas respecto a densidad de semáforos.
*/
int ex_flow_vs_semaphore_density(ExParam p);


int ex_escape_time_vs_density(ExParam p);

int ex_escape_time_vs_rand_prob(ExParam p);

int ex_escape_time_vs_vmax(ExParam p);

int ex_discharge_vs_density(ExParam p);

int ex_discharge_vs_density_fratal(ExParam p);

int ex_dimension_vs_density(ExParam p);

////////////////////////////////////////////////////////////////////////////

/**
* @brief Mide la ocupación en todas las casillas de AC.
* @param ca Puntero al AC sobre el cual se desea medir la ocupación.
* @param out_file_name Nombre opcional para el archivo de salida.
*/
int ex_ocupancy_fixed(CellularAutomata *ca, std::string path = "", std::string out_file_name = "");

/**
* @brief Mide el flujo en todas las casillas de AC.
* @param ca Puntero al AC sobre el cual se desea medir la ocupación.
* @param out_file_name Nombre opcional para el archivo de salida.
*/
int ex_flow_fixed(CellularAutomata *ca, std::string path = "", std::string out_file_name = "");

/**
* @brief Mide el flujo promedio de todas las casillas respecto a densidad de autos.
* @param type Tipo de AC.
* @param size Tamaño de AC.
* @param iterations Número de iteraciones para evolucionar AC.
* @param vmax Velocidad máxima de autos.
* @param density_min Densidad mínima.
* @param density_max Densidad máxima.
* @param dt Intervalo entre densidades.
* @param rand_prob Probabilidad de descenso de velocidad.
* @param extra1 Parámetro extra que puede requerir el AC según el tipo.
* @param extra2 Parámetro extra que puede requerir el AC según el tipo.
* @param per_density Realizar el experimento midiendo el flujo/densidad en vez de flujo.
* @param out_file_name Nombre opcional para el archivo de salida.
*/
int ex_flow_vs_density(const CA_TYPE &type, const unsigned &size, const unsigned &iterations, const int &vmax,
                       const double &density_min, const double &density_max, const double &dt, const double &rand_prob,
                       Args args, const bool &per_density, const int &random_seed = -1, const bool &show_progress = true,
                       std::string path = "", std::string out_file_name = "");

/**
* @brief Mide el flujo promedio de todas las casillas respecto a densidad de autos.
* @param type Tipo de AC.
* @param size Tamaño de AC.
* @param iterations Número de iteraciones para evolucionar AC.
* @param vmax Velocidad máxima de autos.
* @param density_min Densidad mínima.
* @param density_max Densidad máxima.
* @param dt Intervalo entre densidades.
* @param rand_prob Probabilidad de descenso de velocidad.
* @param extra1 Parámetro extra que puede requerir el AC según el tipo.
* @param extra2 Parámetro extra que puede requerir el AC según el tipo.
* @param per_density Realizar el experimento midiendo el flujo/densidad en vez de flujo.
* @param out_file_name Nombre opcional para el archivo de salida.
*/
int ex_multilane_flow_vs_density(const CA_TYPE &type, const unsigned &size, const unsigned &lanes, const unsigned &iterations,
                                 const int &vmax, const double &density_min, const double &density_max, const double &dt,
                                 const double &rand_prob, Args args, const bool &per_density, const int &random_seed = -1,
                                 const bool &show_progress = true, std::string path = "", std::string out_file_name = "");

/**
* @brief Mide el flujo promedio de todas las casillas respecto a velocidad máxima autos.
* @param type Tipo de AC.
* @param size Tamaño de AC.
* @param iterations Número de iteraciones para evolucionar AC.
* @param vmax_min Velocidad máxima de autos mínima.
* @param vmax_max Velocidad máxima de autos máxima.
* @param dt Intervalo entre valores a recorrer.
* @param density Densidad de autos.
* @param rand_prob Probabilidad de descenso de velocidad.
* @param extra1 Parámetro extra que puede requerir el AC según el tipo.
* @param extra2 Parámetro extra que puede requerir el AC según el tipo.
* @param out_file_name Nombre opcional para el archivo de salida.
*/
int ex_flow_vs_vmax(const CA_TYPE &type, const unsigned &size, const unsigned &iterations, const int &vmax_min,
                    const int &vmax_max, const int &dt, const double &density, const double &rand_prob,
                     Args args, const int &random_seed = -1, const bool &show_progress = true, std::string path = "",
                     std::string out_file_name = "");

/**
* @brief Mide el flujo promedio de todas las casillas respecto a velocidad máxima autos.
* @param type Tipo de AC.
* @param size Tamaño de AC.
* @param iterations Número de iteraciones para evolucionar AC.
* @param vmax Velocidad máxima de autos.
* @param density Densidad de autos.
* @param rand_prob_min Probabilidad de descenso de velocidad mínima.
* @param rand_prob_max Probabilidad de descenso de velocidad máxima.
* @param dt Intervalo entre valores a recorrer.
* @param extra1 Parámetro extra que puede requerir el AC según el tipo.
* @param extra2 Parámetro extra que puede requerir el AC según el tipo.
* @param out_file_name Nombre opcional para el archivo de salida.
*/
int ex_flow_vs_rand_prob(const CA_TYPE &type, const unsigned &size, const unsigned &iterations, const int &vmax,
                         const double &density, const double &rand_prob_min, const double &rand_prob_max,
                         const double &dt, Args args, const int &random_seed = -1, const bool &show_progress = true,
                         std::string path = "", std::string out_file_name = "");

/**
* @brief Mide el flujo promedio de todas las casillas respecto a densidad de autos inteligentes.
* @param size Tamaño de AC.
* @param iterations Número de iteraciones para evolucionar AC.
* @param vmax Velocidad máxima de autos.
* @param density Densidad de autos.
* @param rand_prob Probabilidad de descenso de velocidad.
* @param smart_car_density_min Densidad de autos inteligentes mínima.
* @param smart_car_density_max Densidad de autos inteligentes máxima.
* @param dt Intervalo entre valores a recorrer.
* @param out_file_name Nombre opcional para el archivo de salida.
*/
int ex_flow_vs_aut_cars(const unsigned &size, const unsigned &iterations, const int &vmax, const double &density,
                        const double &rand_prob, const double &aut_car_density_min, const double &aut_car_density_max,
                        const double &dt, const int &random_seed = -1, const bool &show_progress = true,
                        std::string path = "", std::string out_file_name = "");

/**
* @brief Mide el flujo promedio de todas las casillas respecto a probabilidad de aparición de nuevo auto.
* @param size Tamaño de AC.
* @param iterations Número de iteraciones para evolucionar AC.
* @param vmax Velocidad máxima de autos.
* @param density Densidad de autos.
* @param rand_prob Probabilidad de descenso de velocidad.
* @param new_car_prob_min Probabilidad de nuevo auto mínima.
* @param new_car_prob_max Probabilidad de nuevo auto máxima.
* @param dt Intervalo entre valores a recorrer.
* @param out_file_name Nombre opcional para el archivo de salida.
*/
int ex_flow_vs_new_car_prob(const CA_TYPE &type, const unsigned &size, const unsigned &iterations, const int &vmax,
                            const double &density, const double &rand_prob, const double &new_car_prob_min,
                            const double &new_car_prob_max, const double &dt, Args args, const bool &per_prob,
                            const int &random_seed = -1, const bool &show_progress = true, std::string path = "",
                            std::string out_file_name = "");

/**
* @brief Mide el flujo promedio de todas las casillas respecto a densidad de topes.
* @param size Tamaño de AC.
* @param iterations Número de iteraciones para evolucionar AC.
* @param vmax Velocidad máxima de autos.
* @param density Densidad de autos.
* @param rand_prob Probabilidad de descenso de velocidad.
* @param stop_density_min Densidad de topes mínima.
* @param stop_density_max Densidad de topes máxima.
* @param dt Intervalo entre valores a recorrer.
* @param out_file_name Nombre opcional para el archivo de salida.
*/
int ex_flow_vs_stop_density(const unsigned &size, const unsigned &iterations, const int &vmax,
                            const double &density, const double &rand_prob, const double &stop_density_min,
                            const double &stop_density_max, const double &dt, const int &random_seed = -1,
                            const bool &show_progress = true, std::string path = "", std::string out_file_name = "");

/**
* @brief Mide el flujo promedio de todas las casillas respecto a densidad de semáforos.
* @param size Tamaño de AC.
* @param iterations Número de iteraciones para evolucionar AC.
* @param vmax Velocidad máxima de autos.
* @param density Densidad de autos.
* @param rand_prob Probabilidad de descenso de velocidad.
* @param semaphore_density_min Densidad de topes mínima.
* @param semaphore_density_max Densidad de topes máxima.
* @param dt Intervalo entre valores a recorrer.
* @param out_file_name Nombre opcional para el archivo de salida.
*/
int ex_flow_vs_semaphore_density(const unsigned &size, const unsigned &iterations, const int &vmax,
                                 const double &density, const double &rand_prob, const double &semaphore_density_min,
                                 const double &semaphore_density_max, const double &dt, const bool &random_semaphores,
                                 const int &random_seed = -1, const bool &show_progress = true, std::string path = "",
                                 std::string out_file_name = "");


int ex_escape_time_vs_density(const CA_TYPE &type, const unsigned &size, const int &vmax,
                              const double &density_min, const double &density_max, const double &dt,
                              const double &rand_prob, Args args, const int &random_seed = -1,
                              const bool &show_progress = true, std::string path = "", std::string out_file_name = "");

int ex_escape_time_vs_rand_prob(const CA_TYPE &type, const unsigned &size, const double &density, const int &vmax,
                                const double &rand_prob_min, const double &rand_prob_max, const double &dt,
                                Args args, const int &random_seed = -1, const bool &show_progress = true,
                                std::string path = "", std::string out_file_name = "");

int ex_escape_time_vs_vmax(const CA_TYPE &type, const unsigned &size, const double &density, const int &vmax_min,
                           const int &vmax_max, const int &dt, const double &rand_prob,
                           Args args, const int &random_seed = -1, const bool &show_progress = true, std::string path = "",
                           std::string out_file_name = "");

int ex_discharge_vs_density(const CA_TYPE &type, const unsigned &size, const int &vmax,
                            const double &density_min, const double &density_max, const double &dt,
                            const double &rand_prob, Args args, const int &random_seed = -1,
                            const bool &show_progress = true, std::string path = "", std::string out_file_name = "");

int ex_discharge_vs_density_fratal(const CA_TYPE &type, const unsigned &size, const int &vmax,
                                   const double &density_min, const double &density_max, const double &dt,
                                   const double &rand_prob, Args args, const int &random_seed = -1,
                                   const bool &show_progress = true, std::string path = "", std::string out_file_name = "");


int ex_dimension_vs_density(const CA_TYPE &type, const unsigned &size, const int &vmax,
                            const double &density_min, const double &density_max, const double &dt,
                            const int &partitions, const double &rand_prob, Args args, const int &random_seed,
                            const bool &show_progress = true, std::string path = "", std::string out_file_name = "");

/**
* @brief Realiza tests para verificar la corrección de los algoritmos.
*/
void ex_perform_test();

#endif
