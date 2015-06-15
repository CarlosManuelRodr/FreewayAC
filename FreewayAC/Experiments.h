/**
* @file Experiments.h
* @brief Experimentos sobre autómatas celulares y funciones de medición.
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
    int vmax, vmax_min, vmax_max;
    int random_seed, init_vel, partitions;

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

    void Report();
};


/****************************
*                           *
*          Medición         *
*                           *
****************************/

/**
* @brief Mide dimensión fractal de sistema unidimensional.
* @param frac Fractal unidimensional.
* @param empty Valor de elemento vacío.
* @param min_div Valor mínimo de divisiones.
* @param max_div Valor máximo de divisiones.
* @param dt_div Intervalo entre las divisiones.
*/
template <class N> double measure_fractal_dimension(std::vector<N> frac, N empty, int min_div, int max_div, int dt_div)
{
    if (min_div == 0 || max_div == 0 || dt_div == 0)
    {
        std::cout << "Error: Valores incorrectos para calculo de dimension fracal." << std::endl;
        std::cout << "min_div = " << min_div << std::endl << "max_div = " << max_div << std::endl;
        std::cout << "dt_div = " << dt_div << std::endl;
        return -1.0;
    }

    // Conteo de cajas.
    std::vector<double> log_epsilon, log_count;
    for (int div = min_div; div <= max_div; div += dt_div)
    {
        double n = 0.0;
        double epsilon = (double)frac.size() / (double)div;
        for (int ex = 0; ex < div; ++ex)
        {
            for (int w = (int)(ex*epsilon); w < (int)((ex + 1)*epsilon); ++w)
            {
                if ((unsigned)w < frac.size())
                {
                    if (frac[w] != empty)
                    {
                        n++;
                        break;
                    }
                }
                else
                    break;
            }
        }
        log_epsilon.push_back(log(1.0 / epsilon));
        log_count.push_back(log(n));
    }

    // Ajuste por mínimos cuadrados.
    double size, sum_xy, sum_x, sum_y, sum_x_squared;
    sum_xy = sum_x = sum_y = sum_x_squared = 0;
    size = log_epsilon.size();
    for (int i = 0; i < size; ++i)
    {
        sum_xy += log_epsilon[i] * log_count[i];
        sum_x += log_epsilon[i];
        sum_y += log_count[i];
        sum_x_squared += pow(log_epsilon[i], 2.0);
    }
    double fit = (size*sum_xy - sum_x*sum_y) / (size*sum_x_squared - pow(sum_x, 2.0));
    return fit;
}

/**
* @brief Mide dimensión fractal de sistema bidimensional.
* @param frac Fractal bidimensional.
* @param empty Valor de elemento vacío.
* @param min_div Valor mínimo de divisiones.
* @param max_div Valor máximo de divisiones.
* @param dt_div Intervalo entre las divisiones.
*/
template <class N> double measure_fractal_dimension(Matrix<N> frac, N empty, int min_div, int max_div, int dt_div)
{
    if (min_div == 0 || max_div == 0 || dt_div == 0)
    {
        std::cout << "Error: Valores incorrectos para calculo de dimension fracal." << std::endl;
        return -1.0;
    }

    if (frac.GetRows() != frac.GetColumns())
    {
        std::cout << "Error: No se puede calcular dimension. El fractal no es cuadrado." << std::endl;
        return -1.0;
    }

    unsigned m_size = frac.GetRows();

    // Conteo de cajas.
    std::vector<double> log_epsilon, log_count;

    for (int div = min_div; div <= max_div; div += dt_div)
    {
        double n = 0.0;
        double epsilon = (double)m_size / (double)div;
        for (int ey = 0; ey < div; ++ey)
        {
            for (int ex = 0; ex < div; ++ex)
            {
                bool found = false;
                for (int w = (int)(ex*epsilon); w < (int)((ex + 1)*epsilon) && !found; ++w)
                {
                    for (int h = (int)(ey*epsilon); h < (int)((ey + 1)*epsilon); h++)
                    {
                        if ((unsigned)w < m_size && (unsigned)h < m_size)
                        {
                            if (frac[w][h] != empty)
                            {
                                n++;
                                found = true;
                                break;
                            }
                        }
                        else
                            break;
                    }
                }
            }
        }
        log_epsilon.push_back(log(1.0 / epsilon));
        log_count.push_back(log(n));
    }

    // Ajuste por mínimos cuadrados.
    double size, sum_xy, sum_x, sum_y, sum_x_squared;
    sum_xy = sum_x = sum_y = sum_x_squared = 0;
    size = log_epsilon.size();
    for (int i = 0; i < size; ++i)
    {
        sum_xy += log_epsilon[i] * log_count[i];
        sum_x += log_epsilon[i];
        sum_y += log_count[i];
        sum_x_squared += pow(log_epsilon[i], 2.0);
    }
    double fit = (size*sum_xy - sum_x*sum_y) / (size*sum_x_squared - pow(sum_x, 2.0));
    return fit;
}
template <class N> double measure_plot_fractal_dimension(std::vector<N> data, int min_div, int max_div, int dt_div)
{
    // Crea matriz donde se guardará la gráfica.
    unsigned coord_y;
    unsigned int size = data.size();
    double min_y = static_cast<double>(*max_element(data.begin(), data.end()));
    double max_y = static_cast<double>(*min_element(data.begin(), data.end()));
    double y_factor = (max_y - min_y) / (size - 1);

    Matrix<bool> plot(size, size);
    plot.Assign(false);

    // Asigna valores a grafica.
    for (unsigned x = 0; x < size; ++x)
    {
        coord_y = (unsigned)((max_y - data[x]) / y_factor);
        plot[x][coord_y] = true;
    }

    return measure_fractal_dimension(plot, false, min_div, max_div, dt_div);
}


/****************************
*                           *
*        Experimentos       *
*                           *
****************************/

/**
* @brief Crea mapa de tráfico.
*/
int ex_traffic_map(ExParam p);

/**
* @brief Crea mapa de flujo.
*/
int ex_flow_map(ExParam p);

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

/**
* @brief Mide el tiempo de escape respecto a densidad de vehículos.
*/
int ex_escape_time_vs_density(ExParam p);

/**
* @brief Mide el tiempo de escape respecto a probabilidad de descenso de velocidad.
*/
int ex_escape_time_vs_rand_prob(ExParam p);

/**
* @brief Mide el tiempo de escape respecto a velocidad máxima.
*/
int ex_escape_time_vs_vmax(ExParam p);

/**
* @brief Mide el gasto respecto a densidad de vehículos.
*/
int ex_discharge_vs_density(ExParam p);

/**
* @brief Crea fractal de gasto respecto a densidad de vehículos.
*/
int ex_discharge_vs_density_fratal(ExParam p);

/**
* @brief Crea gráfica de dimensión fractal de gasto respecto a densidad de vehículos.
*/
int ex_dimension_vs_density(ExParam p);

/**
* @brief Crea gráfica de dimensión fractal de gasto respecto a densidad de vehículos usando threads.
*/
int ex_dimension_vs_density_parallel(ExParam p);

/**
* @brief Realiza tests para verificar la corrección de los algoritmos.
*/
void ex_perform_test();

#endif
