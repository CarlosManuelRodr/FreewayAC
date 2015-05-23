#include <sstream>
#include <string>
#include "CellularAutomata.h"
#include "optionparser.h"
using namespace std;


/****************************
*                           *
*        Experimentos       *
*                           *
****************************/

/**
* @brief Mide la ocupación en todas las casillas de AC.
* @param ca Puntero al AC sobre el cual se desea medir la ocupación.
* @param out_file_name Nombre opcional para el archivo de salida.
*/
void ex_ocupancy_fixed(CellularAutomata *ca, string out_file_name = "")
{
    vector<double> ocupancy;
    ocupancy.assign(ca->GetSize(), 0.0);
    unsigned height = ca->GetHistorySize();
    unsigned width = ca->GetSize();

    for (unsigned i=0; i<width; ++i)
    {
        int sum = 0;
        for (unsigned j=0; j<height; ++j)
        {
            if (ca->At(i, j, CA_HISTORY) != -1)
                sum++;
        }
        ocupancy[i] = (double)sum/(double)height;
    }

    // Escribe a CSV.
    if (out_file_name.empty())
        out_file_name = "ocupancy.csv";
    ofstream file(out_file_name.c_str(), ofstream::out);
    for (unsigned i = 0; i < ocupancy.size(); ++i)
    {
        if (i - ocupancy.size() != 0)
            file << ocupancy[i] << ", ";
        else
            file << ocupancy[i];
    }

    file.close();
}

/**
* @brief Mide el flujo en todas las casillas de AC.
* @param ca Puntero al AC sobre el cual se desea medir la ocupación.
* @param out_file_name Nombre opcional para el archivo de salida.
*/
void ex_flow_fixed(CellularAutomata *ca, string out_file_name = "")
{
    vector<double> flow;
    flow.assign(ca->GetSize(), 0.0);
    unsigned height = ca->GetHistorySize();
    unsigned width = ca->GetSize();

    for (unsigned i=0; i<width; ++i)
    {
        int sum = 0;
        for (unsigned j=0; j<height; ++j)
        {
            if ((ca->At(i, j, CA_FLOW_HISTORY) != 0) && (ca->At(i+1, j, CA_FLOW_HISTORY) != 0)) 
                sum++;
        }
        flow[i] = (double)sum/(double)height;
    }

    // Escribe a CSV.
    if (out_file_name.empty())
        out_file_name = "flow.csv";
    ofstream file(out_file_name.c_str(), ofstream::out);
    for (unsigned i = 0; i < flow.size(); ++i)
    {
        if (i - flow.size() != 0)
            file << flow[i] << ", ";
        else
            file << flow[i];
    }

    file.close();
}

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
void ex_flow_vs_density(const CA_TYPE &type, const unsigned &size, const unsigned &iterations, const int &vmax, 
                        const double &density_min, const double &density_max, const double &dt, const double &rand_prob, 
                        Args &args, const bool &per_density = false, string out_file_name = "")
{
    vector<double> density;
    vector<double> flow;
    CellularAutomata* ca;

    for (double d=density_min; d<=density_max; d+=dt)
    {
        // Reporta progreso.
        if (d + dt > density_max)
            aux_progress_bar(1.0);
        else
            aux_progress_bar(d/density_max);

        // Evoluciona el sistema.
        ca = create_ca(type, size, d, vmax, rand_prob, args);
        for (unsigned i=0; i<iterations; ++i)
            ca->Step();


        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(size, 0.0);
        unsigned height = iterations;
        unsigned width = size;

        for (unsigned i=0; i<width; ++i)
        {
            int sum = 0;
            for (unsigned j=0; j<height; ++j)
            {
                if ((ca->At(i, j, CA_FLOW_HISTORY) != 0) && (ca->At(i+1, j, CA_FLOW_HISTORY) != 0))
                    sum++;
            }
            tmp_flow[i] = (double)sum/(double)height;
        }

        // Obtiene el promedio de todos los flujos.
        double mean = 0;
        for (unsigned i=0; i<tmp_flow.size(); ++i)
            mean += tmp_flow[i];
        mean /= (double)tmp_flow.size();
        if (per_density) mean /= d;

        // Asigna valores.
        density.push_back(d);
        flow.push_back(mean);
    }

    // Escribe a CSV.
    if (out_file_name.empty())
    {
        if (per_density)
            out_file_name = "flow_per_density.csv";
        else
            out_file_name = "flow_vs_density.csv";
    }

    ofstream file(out_file_name.c_str(), ofstream::out);
    for (unsigned i = 0; i < flow.size(); ++i)
    {
        if (i - flow.size() != 0)
            file << density[i] << ", " << flow[i] << endl;
        else
            file << density[i] << ", " << flow[i];
    }
    file.close();
    delete_ca();
}

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
void ex_flow_vs_vmax(const CA_TYPE &type, const unsigned &size, const unsigned &iterations, const int &vmax_min,
                     const int &vmax_max, const int &dt, const double &density, const double &rand_prob,
					 Args &args, string out_file_name = "")
{
    vector<double> vmax;
    vector<double> flow;
    CellularAutomata* ca;

    for (int v=vmax_min; v<=vmax_max; v+=dt)
    {
        // Reporta progreso.
        if (v + dt > vmax_max)
            aux_progress_bar(1.0);
        else
            aux_progress_bar((double)v/(double)vmax_max);

        // Evoluciona el sistema.
		ca = create_ca(type, size, density, v, rand_prob, args);
        for (unsigned i=0; i<iterations; ++i)
            ca->Step();


        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(size, 0.0);
        unsigned height = iterations;
        unsigned width = size;

        for (unsigned i=0; i<width; ++i)
        {
            int sum = 0;
            for (unsigned j=0; j<height; ++j)
            {
                if ((ca->At(i, j, CA_FLOW_HISTORY) != 0) && (ca->At(i+1, j, CA_FLOW_HISTORY) != 0))
                    sum++;
            }
            tmp_flow[i] = (double)sum/(double)height;
        }

        // Obtiene el promedio de todos los flujos.
        double mean = 0;
        for (unsigned i=0; i<tmp_flow.size(); ++i)
            mean += tmp_flow[i];
        mean /= (double)tmp_flow.size();

        // Asigna valores.
        vmax.push_back(v);
        flow.push_back(mean);
    }

    // Escribe a CSV.
    if (out_file_name.empty())
        out_file_name = "flow_vs_vmax.csv";
    ofstream file(out_file_name.c_str(), ofstream::out);
    for (unsigned i = 0; i < flow.size(); ++i)
    {
        if (i - flow.size() != 0)
            file << vmax[i] << ", " << flow[i] << endl;
        else
            file << vmax[i] << ", " << flow[i];
    }
    file.close();
    delete_ca();
}

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
void ex_flow_vs_rand_prob(const CA_TYPE &type, const unsigned &size, const unsigned &iterations, const int &vmax,
                          const double &density, const double &rand_prob_min, const double &rand_prob_max,
						  const double &dt, Args &args, string out_file_name = "")
{
    vector<double> rand_prob;
    vector<double> flow;
    CellularAutomata* ca;

    for (double r=rand_prob_min; r<=rand_prob_max; r+=dt)
    {
        // Reporta progreso.
        if (r + dt > rand_prob_max)
            aux_progress_bar(1.0);
        else
            aux_progress_bar(r/rand_prob_max);

        // Evoluciona el sistema.
		ca = create_ca(type, size, density, vmax, r, args);
        for (unsigned i=0; i<iterations; ++i)
            ca->Step();


        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(size, 0.0);
        unsigned height = iterations;
        unsigned width = size;

        for (unsigned i=0; i<width; ++i)
        {
            int sum = 0;
            for (unsigned j=0; j<height; ++j)
            {
                if ((ca->At(i, j, CA_FLOW_HISTORY) != 0) && (ca->At(i+1, j, CA_FLOW_HISTORY) != 0))
                    sum++;
            }
            tmp_flow[i] = (double)sum/(double)height;
        }

        // Obtiene el promedio de todos los flujos.
        double mean = 0;
        for (unsigned i=0; i<tmp_flow.size(); ++i)
            mean += tmp_flow[i];
        mean /= (double)tmp_flow.size();

        // Asigna valores.
        rand_prob.push_back(r);
        flow.push_back(mean);
    }

    // Escribe a CSV.
    if (out_file_name.empty())
        out_file_name = "flow_vs_rand_prob.csv";
    ofstream file(out_file_name.c_str(), ofstream::out);
    for (unsigned i = 0; i < flow.size(); ++i)
    {
        if (i - flow.size() != 0)
            file << rand_prob[i] << ", " << flow[i] << endl;
        else
            file << rand_prob[i] << ", " << flow[i];
    }
    file.close();
    delete_ca();
}

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
void ex_flow_vs_smart_cars(const unsigned &size, const unsigned &iterations, const int &vmax, const double &density, 
                           const double &rand_prob, const double &smart_car_density_min, const double &smart_car_density_max,
                           const double &dt, string out_file_name = "")
{
    vector<double> smart_car_density;
    vector<double> flow;
    CellularAutomata* ca;

    for (double s=smart_car_density_min; s<=smart_car_density_max; s+=dt)
    {
        // Reporta progreso.
        if (s + dt > smart_car_density_max)
            aux_progress_bar(1.0);
        else
            aux_progress_bar(s/smart_car_density_max);

        // Evoluciona el sistema.
		ca = create_ca(SMART_CA, size, density, vmax, rand_prob, Args({ s }));
        for (unsigned i=0; i<iterations; ++i)
            ca->Step();


        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(size, 0.0);
        unsigned height = iterations;
        unsigned width = size;

        for (unsigned i=0; i<width; ++i)
        {
            int sum = 0;
            for (unsigned j=0; j<height; ++j)
            {
                if ((ca->At(i, j, CA_FLOW_HISTORY) != 0) && (ca->At(i+1, j, CA_FLOW_HISTORY) != 0))
                    sum++;
            }
            tmp_flow[i] = (double)sum/(double)height;
        }

        // Obtiene el promedio de todos los flujos.
        double mean = 0;
        for (unsigned i=0; i<tmp_flow.size(); ++i)
            mean += tmp_flow[i];
        mean /= (double)tmp_flow.size();

        // Asigna valores.
        smart_car_density.push_back(s);
        flow.push_back(mean);
    }

    // Escribe a CSV.
    if (out_file_name.empty())
        out_file_name = "flow_vs_smart_car_density.csv";
    ofstream file(out_file_name.c_str(), ofstream::out);
    for (unsigned i = 0; i < flow.size(); ++i)
    {
        if (i - flow.size() != 0)
            file << smart_car_density[i] << ", " << flow[i] << endl;
        else
            file << smart_car_density[i] << ", " << flow[i];
    }
    file.close();
    delete_ca();
}

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
void ex_flow_vs_new_car_prob(const unsigned &size, const unsigned &iterations, const int &vmax, const double &density, 
                             const double &rand_prob, const double &new_car_prob_min, const double &new_car_prob_max,
                             const double &dt, const int &new_car_speed, string out_file_name = "")
{
    vector<double> new_car_density;
    vector<double> flow;
    CellularAutomata* ca;

    for (double s=new_car_prob_min; s<=new_car_prob_max; s+=dt)
    {
        // Reporta progreso.
        if (s + dt > new_car_prob_max)
            aux_progress_bar(1.0);
        else
            aux_progress_bar(s/new_car_prob_max);

        // Evoluciona el sistema.
		ca = create_ca(OPEN_CA, size, density, vmax, rand_prob, Args({ s }, { new_car_speed }));
        for (unsigned i=0; i<iterations; ++i)
            ca->Step();


        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(size, 0.0);
        unsigned height = iterations;
        unsigned width = size;

        for (unsigned i=0; i<width; ++i)
        {
            int sum = 0;
            for (unsigned j=0; j<height; ++j)
            {
                if ((ca->At(i, j, CA_FLOW_HISTORY) != 0) && (ca->At(i+1, j, CA_FLOW_HISTORY) != 0))
                    sum++;
            }
            tmp_flow[i] = (double)sum/(double)height;
        }

        // Obtiene el promedio de todos los flujos.
        double mean = 0;
        for (unsigned i=0; i<tmp_flow.size(); ++i)
            mean += tmp_flow[i];
        mean /= (double)tmp_flow.size();
        mean /= s;

        // Asigna valores.
        new_car_density.push_back(s);
        flow.push_back(mean);
    }

    // Escribe a CSV.
    if (out_file_name.empty())
        out_file_name = "flow_vs_new_density.csv";
    ofstream file(out_file_name.c_str(), ofstream::out);
    for (unsigned i = 0; i < flow.size(); ++i)
    {
        if (i - flow.size() != 0)
            file << new_car_density[i] << ", " << flow[i] << endl;
        else
            file << new_car_density[i] << ", " << flow[i];
    }
    file.close();
    delete_ca();
}

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
void ex_flow_vs_stop_density(const unsigned &size, const unsigned &iterations, const int &vmax,
                             const double &density, const double &rand_prob, const double &stop_density_min,
                             const double &stop_density_max, const double &dt, string out_file_name = "")
{
    vector<double> stop_density;
    vector<double> flow;
    CellularAutomata* ca;

    for (double d=stop_density_min; d<=stop_density_max; d+=dt)
    {
        // Reporta progreso.
        if (d + dt > stop_density_max)
            aux_progress_bar(1.0);
        else
            aux_progress_bar(d/stop_density_max);

        // Evoluciona el sistema.
		ca = create_ca(STOP_CA, size, density, vmax, rand_prob, Args({ d }));
        for (unsigned i=0; i<iterations; ++i)
            ca->Step();


        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(size, 0.0);
        unsigned height = iterations;
        unsigned width = size;

        for (unsigned i=0; i<width; ++i)
        {
            int sum = 0;
            for (unsigned j=0; j<height; ++j)
            {
                if ((ca->At(i, j, CA_FLOW_HISTORY) != 0) && (ca->At(i+1, j, CA_FLOW_HISTORY) != 0))
                    sum++;
            }
            tmp_flow[i] = (double)sum/(double)height;
        }

        // Obtiene el promedio de todos los flujos.
        double mean = 0;
        for (unsigned i=0; i<tmp_flow.size(); ++i)
            mean += tmp_flow[i];
        mean /= (double)tmp_flow.size();

        // Asigna valores.
        stop_density.push_back(d);
        flow.push_back(mean);
    }

    // Escribe a CSV.
    if (out_file_name.empty())
        out_file_name = "flow_vs_stop_density.csv";
    ofstream file(out_file_name.c_str(), ofstream::out);
    for (unsigned i = 0; i < flow.size(); ++i)
    {
        if (i - flow.size() != 0)
            file << stop_density[i] << ", " << flow[i] << endl;
        else
            file << stop_density[i] << ", " << flow[i];
    }
    file.close();
    delete_ca();
}

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
void ex_flow_vs_semaphore_density(const unsigned &size, const unsigned &iterations, const int &vmax,
                                  const double &density, const double &rand_prob, const double &semaphore_density_min,
                                  const double &semaphore_density_max, const double &dt, const bool &random_semaphores, 
                                  string out_file_name = "")
{
    vector<double> semaphore_density;
    vector<double> flow;
    CellularAutomata* ca;

    for (double d=semaphore_density_min; d<=semaphore_density_max; d+=dt)
    {
        // Reporta progreso.
        if (d + dt > semaphore_density_max)
            aux_progress_bar(1.0);
        else
            aux_progress_bar(d/semaphore_density_max);

        // Evoluciona el sistema.
		ca = create_ca(SEMAPHORE_CA, size, density, vmax, rand_prob, Args({ d }, {}, { random_semaphores }));
        for (unsigned i=0; i<iterations; ++i)
            ca->Step();


        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(size, 0.0);
        unsigned height = iterations;
        unsigned width = size;

        for (unsigned i=0; i<width; ++i)
        {
            int sum = 0;
            for (unsigned j=0; j<height; ++j)
            {
                if ((ca->At(i, j, CA_FLOW_HISTORY) != 0) && (ca->At(i+1, j, CA_FLOW_HISTORY) != 0))
                    sum++;
            }
            tmp_flow[i] = (double)sum/(double)height;
        }

        // Obtiene el promedio de todos los flujos.
        double mean = 0;
        for (unsigned i=0; i<tmp_flow.size(); ++i)
            mean += tmp_flow[i];
        mean /= (double)tmp_flow.size();

        // Asigna valores.
        semaphore_density.push_back(d);
        flow.push_back(mean);
    }

    // Escribe a CSV.
    if (out_file_name.empty())
        out_file_name = "flow_vs_semaphore_density.csv";
    ofstream file(out_file_name.c_str(), ofstream::out);
    for (unsigned i = 0; i < flow.size(); ++i)
    {
        if (i - flow.size() != 0)
            file << semaphore_density[i] << ", " << flow[i] << endl;
        else
            file << semaphore_density[i] << ", " << flow[i];
    }
    file.close();
    delete_ca();
}

/**
* @brief Realiza tests para verificar la corrección de los algoritmos.
*/
void perform_test()
{
    const int init[] = {1, -1, -1, 1, -1, 1, 1, -1, -1, 1, -1, -1, -1, 1, -1, -1, 1, -1, -1, -1};

    // Circular.
    const bool rand_val_circ[] = {false, false, false, false, true, false, true, 
                                  true, false, false, true, false, false, false, 
                                  false, true, false, false, true, false, false};
    const int end_circ[] = {-1, 2, -1, 1, 0, -1, -1, 1, -1, 1, -1,-1, -1, -1, 2, -1, -1, -1, 2, -1};
    vector<int> init_vec(init, init + sizeof(init) / sizeof(init[0]));
    vector<bool> rand_val_circ_vec(rand_val_circ, rand_val_circ + sizeof(rand_val_circ) / sizeof(rand_val_circ[0]));

    cout << "Comprobando automata celular circular... ";
    CircularCA circ_ca(init_vec, rand_val_circ_vec, 5);
    circ_ca.Evolve(3);

    bool match = true;
    for (unsigned i=0; i<init_vec.size(); i++)
    {
        if (circ_ca.At(i) != end_circ[i])
            match = false;
    }

    if (match)
        cout << "Correcto!" << endl;
    else
    {
        cout << "Incorrecto. Log:" << endl;
        circ_ca.PrintHistory();
    }

    // Abierto.
    const bool rand_val_open[] = {false, false, false, false, true, false, true, false,
                                  true, false, false, true, false, false, false, true,
                                  false, true, false, false, true, false, false, false};
    const int end_open[] = {-1, 1, 0, -1, -1, 1, -1, 1, 0, -1, -1,-1, -1, -1, -1, 3, -1, -1, 2, -1};
    vector<bool> rand_val_open_vec(rand_val_open, rand_val_open + sizeof(rand_val_open) / sizeof(rand_val_open[0]));

    cout << "Comprobando automata celular abierto... ";
    OpenCA open_ca(init_vec, rand_val_open_vec, 5, 1);
    open_ca.Evolve(3);

    match = true;
    for (unsigned i=0; i<init_vec.size(); i++)
    {
        if (open_ca.At(i) != end_open[i])
            match = false;
    }

    if (match)
        cout << "Correcto!" << endl;
    else
    {
        cout << "Incorrecto. Log:" << endl;
        open_ca.PrintHistory();
    }
}


/****************************
*                           *
*   Parser de argumentos    *
*                           *
****************************/

struct Arg: public option::Arg
{
    static void PrintError(const char* msg1, const option::Option& opt, const char* msg2)
    {
        fprintf(stderr, "%s", msg1);
        fwrite(opt.name, opt.namelen, 1, stderr);
        fprintf(stderr, "%s", msg2);
    }

    static option::ArgStatus Required(const option::Option& option, bool msg)
    {
        if (option.arg != 0)
            return option::ARG_OK;

        if (msg) PrintError("Opcion '", option, "' requiere un argumento.\n");
        return option::ARG_ILLEGAL;
    }
};

enum  OptionIndex { UNKNOWN, SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, PLOT_TRAFFIC, MEASURE_OCUPANCY, MEASURE_FLOW, 
                    FLOW_VS_DENSITY, FLOW_PER_DENSITY, FLOW_VS_VMAX, FLOW_VS_RAND_PROB, FLOW_VS_SMART_CARS, 
					FLOW_VS_STOP_DENSITY, FLOW_VS_NEW_CAR, FLOW_VS_SEMAPHORE_DENSITY, CA_CIRCULAR, CA_OPEN, 
					CA_SMART, CA_STOP, CA_SEMAPHORE, CA_SIMPLE_JUNCTION, NEW_CAR_PROB, NEW_CAR_SPEED, SMART_DENSITY, 
					STOP_DENSITY, SEMAPHORE_DENSITY, RANDOM_SEMAPHORES, DT, DMIN, DMAX, VMAX_MIN, VMAX_MAX, 
					RAND_PROB_MIN, SMART_MIN, SMART_MAX, STOP_DENSITY_MIN, STOP_DENSITY_MAX, NEW_CAR_MIN, 
					NEW_CAR_MAX, RAND_PROB_MAX, SEMAPHORE_DENSITY_MIN, SEMAPHORE_DENSITY_MAX, OUT_FILE_NAME, 
					TEST, HELP };

const option::Descriptor usage[] =
{
    {UNKNOWN, 0, "", "",Arg::None, "INSTRUCCIONES: FreewayAC [opciones]\n"},
    {SIZE,  0,"s", "size", Arg::Required, "  -s <arg>, \t--size=<arg>  \tTamagno del AC." },
    {ITERATIONS,  0,"i", "iter", Arg::Required, "  -i <arg>, \t--iter=<arg>  \tIteraciones del AC." },
    {VMAX,  0,"v", "vmax", Arg::Required, "  -v <arg>, \t--vmax=<arg>  \tVelocidad maxima de auto." },
    {DENSITY,  0,"d", "density", Arg::Required, "  -d <arg>, \t--density=<arg>  \tDensidad de autos." },
    {RAND_PROB,  0,"r", "rand_prob", Arg::Required, "  -r <arg>, \t--rand_prob=<arg>  \tProbabilidad de descenso de velocidad." },
    {PLOT_TRAFFIC,  0,"p","plot_traffic", Arg::None, "  -p , \t--plot_traffic  \tCrea mapa de autos vs tiempo y flujo vs tiempo." },
    {MEASURE_OCUPANCY,  0,"o","ocupancy_fixed", Arg::None, "  -o , \t--ocupancy_fixed  \tMide la densidad de ocupacion promedio de cada casilla." },
    {MEASURE_FLOW,  0,"f","flow_fixed", Arg::None, "  -f , \t--flow_fixed  \tMide el flujo promedio de cada casilla." },
    {FLOW_VS_DENSITY,  0,"","flow_vs_density", Arg::None, 
    "  \t--flow_vs_density  \tMide el flujo respecto a densidad en un rango especificado por dmin, dmax y dt." },
    {FLOW_PER_DENSITY,  0,"","flow_per_density", Arg::None, 
    "  \t--flow_per_density  \tMide el flujo por densidad respecto a densidad en un rango especificado por dmin, dmax y dt." },
    {FLOW_VS_VMAX,  0,"","flow_vs_vmax", Arg::None, 
    "  \t--flow_vs_vmax  \tMide el flujo respecto a vmax en un rango especificado por vmax_min, vmax_max y dt." },
    {FLOW_VS_RAND_PROB,  0,"","flow_vs_rand_prob", Arg::None, 
     "  \t--flow_vs_rand_prob  \tMide el flujo respecto a probabilidad de frenado en un rango especificado por rand_min, rand_max y dt." },
    {FLOW_VS_SMART_CARS,  0,"","flow_vs_smart_cars", Arg::None, 
     "  \t--flow_vs_smart_cars  \tMide el flujo respecto a densidad de autos inteligentes en un rango especificado por smart_min, smart_max y dt." },
    {FLOW_VS_STOP_DENSITY,  0,"","flow_vs_stop_density", Arg::None,
          "  \t--flow_vs_stop_density  \tMide el flujo respecto a densidad de topes en un rango especificado por stop_density_min, stop_density_max y dt." },
    {FLOW_VS_NEW_CAR,  0,"","flow_vs_new_car", Arg::None, 
     "  \t--flow_vs_new_car  \tFlujo respecto a probabilidad de nuevo auto en ac abierto en un rango especificado por new_car_min, new_car_max y dt." },
    {FLOW_VS_SEMAPHORE_DENSITY,  0,"","flow_vs_semaphore_density", Arg::None, 
     "  \t--flow_vs_new_car  \tFlujo respecto a densidad de semaforos en un rango especificado por semaphore_density_min y semaphore_density_max." },
    {CA_CIRCULAR,  0,"","ca_circular", Arg::None, "  \t--ca_circular  \tAutomata celular circular." },
    {CA_OPEN,  0,"","ca_open", Arg::None, "  \t--ca_open  \tAutomata celular con frontera abierta." },
    {CA_SMART,  0,"","ca_smart", Arg::None, "  \t--ca_smart  \tAutomata celular con autos inteligentes." },
    {CA_STOP,  0,"","ca_stop", Arg::None,
     "  \t--ca_stop  \tAutomata celular con tope. La posicion del tope se especifica por stop_density." },
    {CA_SEMAPHORE,  0,"","ca_semaphore", Arg::None, 
     "  \t--ca_semaphore  \tAutomata celular con semaforo. La posicion del semaforo se especifica por semaphore_density." },
	{CA_SIMPLE_JUNCTION, 0, "", "ca_simple_junction", Arg::None,
	 "  \t--ca_simple_junction  \tAutomata de interseccion simple." },
    {NEW_CAR_PROB,  0,"","new_car_prob", Arg::Required, "  \t--new_car_prob  \tProbabilidad de que se aparezca nuevo auto en frontera abierta." },
	{NEW_CAR_SPEED, 0, "", "new_car_speed", Arg::Required, "  \t--new_car_speed  \tVelocidad que entre a AC abierto." },
    {SMART_DENSITY,  0,"","smart_density", Arg::Required, "  \t--smart_density  \tDensidad de autos inteligentes." },
    {STOP_DENSITY,  0,"", "stop_density", Arg::Required, "  \t--stop_density=<arg>  \tDensidad de topes en ca_stop." },
    {SEMAPHORE_DENSITY,  0,"", "semaphore_density", Arg::Required, 
     "  \t--semaphore_density=<arg>  \tDensidad de semaforos en ca_semaphore." },
    {RANDOM_SEMAPHORES,  0,"", "random_semapahores", Arg::None, 
     "  \t--random_semapahores  \tColoca los semaforos en posiciones aleatorias en vez de uniformes." },
    {DT,  0,"", "dt", Arg::Required, "  \t--dt=<arg>  \tTamagno del intervalo en medicion con rango." },
    {DMIN,  0,"", "dmin", Arg::Required, "  \t--dmin=<arg>  \tDensidad minima en medicion con rango." },
    {DMAX,  0,"", "dmax", Arg::Required, "  \t--dmax=<arg>  \tDensidad maxima en medicion con rango." },
    {VMAX_MIN,  0,"", "vmax_min", Arg::Required, "  \t--vmax_min=<arg>  \tVmax minima en medicion con rango." },
    {VMAX_MAX,  0,"", "vmax_max", Arg::Required, "  \t--vmax_max=<arg>  \tVmax maxima en medicion con rango." },
    {RAND_PROB_MIN,  0,"", "rand_prob_min", Arg::Required, "  \t--rand_prob_min=<arg>  \tProbabilidad de frenado minima en medicion con rango." },
    {RAND_PROB_MAX,  0,"", "rand_prob_max", Arg::Required, "  \t--rand_prob_max=<arg>  \tProbabilidad de frenado maxima en medicion con rango." },
    {SMART_MIN,  0,"", "smart_min", Arg::Required, "  \t--smart_min=<arg>  \tDensidad minima de autos inteligentes." },
    {SMART_MAX,  0,"", "smart_max", Arg::Required, "  \t--smart_max=<arg>  \tDensidad maxima de autos inteligentes." },
    {STOP_DENSITY_MIN,  0,"", "stop_density_min", Arg::Required, "  \t--stop_density_min=<arg>  \tDensidad minima de topes." },
    {STOP_DENSITY_MAX,  0,"", "stop_density_max", Arg::Required, "  \t--stop_density_max=<arg>  \tDensidad maxima de topes." },
    {SEMAPHORE_DENSITY_MIN,  0,"", "semaphore_density_min", Arg::Required, "  \t--semaphore_density_min=<arg>  \tDensidad minima de semaforos." },
    {SEMAPHORE_DENSITY_MAX,  0,"", "semaphore_density_max", Arg::Required, "  \t--semaphore_density_max=<arg>  \tDensidad maxima de semaforos." },
    {NEW_CAR_MIN,  0,"", "new_car_min", Arg::Required, "  \t--new_car_min=<arg>  \tProbabilidad minima de nuevo auto en ac abierto." },
    {NEW_CAR_MAX,  0,"", "new_car_max", Arg::Required, "  \t--new_car_max=<arg>  \tProbabilidad maxima de nuevo auto en ac abierto." },
    {OUT_FILE_NAME,  0,"", "out_file_name", Arg::Required, "  \t--out_file_name=<arg>  \tCambia el nombre del archivo de salida al especificado." },
    {TEST,    0,"", "test", Arg::None,    "  \t--test  \tRealiza pruebas para garantizar la fiabilidad de resultados." },
    {HELP,    0,"", "help", Arg::None,    "  \t--help  \tMuestra instrucciones detalladas de cada experimento." },
    {0,0,0,0,0,0}
};

void describe_experiments()
{
    const char *text = "A continuacion se enumera la lista de experimentos y AC disponibles y sus parametros.\n"
                       "Todas las opciones se representan en mayusculas.\n\n"
                       "=== Tipos de automatas celulares ===\n"
                       "CA_CIRCULAR          -> Descripcion: Automata celular con fronteras periodicas. Pista circular.\n"
                       "                        Parametros relevantes: Ninguno.\n"
                       "CA_OPEN              -> Descripcion: Automata celular con fronteras abiertas. Entran autos en\n"
                       "                                     la primera pos del AC.\n"
                       "                        Parametros relevantes: NEW_CAR_PROB, NEW_CAR_SPEED.\n"
                       "CA_SMART             -> Descripcion: Automata celular circular con autos inteligentes."
                       "                        Parametros relevantes: SMART_DENSITY.\n"
                       "CA_STOP              -> Descripcion: Automata celular circular con topes en la pista.\n"
                       "                        Parametros relevantes: STOP_DENSITY.\n"
                       "CA_SEMAPHORE         -> Descripcion: Automata celular circular con semaforos en la pista.\n"
                       "                        Parametros relevantes: SEMAPHORE_DENSITY, RANDOM_SEMAPHORES.\n"
                       "\n=== Experimentos de parametro fijo ===\n"
                       "PLOT_TRAFFIC         -> Descripcion: Evoluciona automata celular y grafica su representacion.\n"
                       "                        Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB.\n"
                       "MEASURE_OCUPANCY     -> Descripcion: Mide la ocupacion de cada casilla del automata celular.\n"
                       "                        Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB.\n"
                       "MEASURE_FLOW         -> Descripcion: Mide el flujo de cada casilla del automata celular.\n"
                       "                        Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB.\n"
                       "\n=== Experimentos de parametro variable ===\n"
                       "FLOW_VS_DENSITY      -> Descripcion: Mide el flujo de autos promedio respecto a los valores\n"
                       "                                     de densidad de autos especificados en un rango.\n"
                       "                        Parametros relevantes: SIZE, ITERATIONS, VMAX, RAND_PROB, DMIN, DMAX, DT.\n"
                       "FLOW_PER_DENSITY     -> Descripcion: Mide el flujo de autos promedio sobre densidad respecto\n"
                       "                                     a los valores de densidad de autos especificados en un rango.\n"
                       "                        Parametros relevantes: SIZE, ITERATIONS, VMAX, RAND_PROB, DMIN, DMAX, DT.\n"
                       "FLOW_VS_VMAX         -> Descripcion: Mide el flujo de autos promedio respecto a los valores de\n"
                       "                                     velocidad maxima especificados en un rango.\n"
                       "                        Parametros relevantes: SIZE, ITERATIONS, DENSITY, RAND_PROB, VMAX_MIN, VMAX_MAX, DT.\n"
                       "FLOW_VS_RAND_PROB    -> Descripcion: Mide el flujo de autos promedio respecto a los valores de\n"
                       "                                     probabilidad de descenso de velocidad especificados en un rango.\n"
                       "                        Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB_MIN,\n"
                       "                                               RAND_PROB_MAX, DT.\n"
                       "FLOW_VS_SMART_CARS   -> Descripcion: Mide el flujo de autos promedio respecto a los valores\n"
                       "                                     de densidad de autos inteligentes especificados en un rango.\n"
                       "                        Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, \n"
                       "                                               SMART_MIN, SMART_MAX, DT.\n"
                       "FLOW_VS_STOP_DENSITY -> Descripcion: Mide el flujo de autos promedio respecto a los valores\n"
                       "                                     densidad de topes especificados en un rango. Se usa el\n"
                       "                                     automata celular CA_STOP.\n"
                       "                        Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB,\n"
                       "                                               STOP_DENSITY_MIN, STOP_DENSITY_MAX, DT.\n"
                       "FLOW_VS_NEW_CAR      -> Descripcion: Mide el flujo de autos promedio respecto a los valores\n"
                       "                                     de probabilidad de aparicion de nuevo auto especificados\n"
                       "                                     en un rango. Se usa el automata celular CA_OPEN.\n"
                       "                        Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB,\n"
                       "                                               NEW_CAR_MIN, NEW_CAR_MAX, DT.\n";
    cout << text << endl;
}


int main(int argc, char* argv[])
{
    unsigned size = 100, iterations = 100;
    int vmax = 5, vmax_min = 0, vmax_max = 20;
    double density = 0.2, rand_prob = 0.2;
    double dt = 0.1, dmin = 0.0, dmax = 1.0, rand_prob_min = 0.0, rand_prob_max = 1.0;
    double smart_min = 0.0, smart_max = 0.7, new_car_min = 0.0, new_car_max = 1.0, stop_density_min = 0.0;
    double stop_density_max = 0.1, semaphore_density_min = 0.0, semaphore_density_max = 1.0;
    bool ocupancy_fixed = false, flow_fixed = false, flow_vs_density = false;
    bool plot_traffic = false, flow_vs_vmax = false, flow_vs_rand_prob = false, flow_vs_smart_cars = false;
    bool flow_vs_stop_density = false, flow_per_density = false, flow_vs_new_car = false, flow_vs_semaphore_density = false;
    bool random_semaphores = false, test = false;
    CA_TYPE ca_type = CIRCULAR_CA;
    double new_car_prob = 0.1, smart_density = 0.1, stop_density = 0.1, semaphore_density = 0.1;
	int new_car_speed = 1;
    string out_file_name = "";

    // Ejecuta parser de argumentos.
    argc -= (argc>0); argv += (argc>0);
    option::Stats  stats(usage, argc, argv);
    option::Option *options = new option::Option[stats.options_max];
    option::Option *buffer = new option::Option[stats.buffer_max];
    option::Parser parse(usage, argc, argv, options, buffer);

    if (parse.error())
        return 1;

    if (options[HELP] || argc == 0)
    {
        if (options[HELP])
            describe_experiments();
        option::printUsage(cout, usage);
        return 0;
    }

    for (option::Option* opt = options[UNKNOWN]; opt; opt = opt->next())
        cout << "Opcion desconocida: " << opt->name << "\n";

    for (int i = 0; i < parse.optionsCount(); ++i)
    {
        option::Option& opt = buffer[i];
        switch (opt.index())
        {
            case SIZE:
            size = aux_string_to_num<unsigned>(opt.arg);
            break;

            case ITERATIONS:
            iterations = aux_string_to_num<unsigned>(opt.arg);
            break;

            case VMAX:
            vmax = aux_string_to_num<int>(opt.arg);
            break;

            case DENSITY:
            density = aux_string_to_num<double>(opt.arg);
            break;

            case RAND_PROB:
            rand_prob = aux_string_to_num<double>(opt.arg);
            break;

            case PLOT_TRAFFIC:
            plot_traffic = true;
            break;

            case MEASURE_OCUPANCY:
            ocupancy_fixed = true;
            break;

            case MEASURE_FLOW:
            flow_fixed = true;
            break;

            case FLOW_VS_DENSITY:
            flow_vs_density = true;
            break;

            case FLOW_PER_DENSITY:
            flow_per_density = true;
            break;

            case FLOW_VS_VMAX:
            flow_vs_vmax = true;
            break;

            case FLOW_VS_RAND_PROB:
            flow_vs_rand_prob = true;
            break;

            case FLOW_VS_SMART_CARS:
            flow_vs_smart_cars = true;
            break;

            case FLOW_VS_STOP_DENSITY:
            flow_vs_stop_density = true;
            break;

            case FLOW_VS_SEMAPHORE_DENSITY:
            flow_vs_semaphore_density = true;
            break;

            case FLOW_VS_NEW_CAR:
            flow_vs_new_car = true;
            break;

            case CA_CIRCULAR:
            ca_type = CIRCULAR_CA;
            break;

            case CA_OPEN:
            ca_type = OPEN_CA;
            break;

            case CA_SMART:
            ca_type = SMART_CA;
            break;

            case CA_STOP:
            ca_type = STOP_CA;
            break;

            case CA_SEMAPHORE:
            ca_type = SEMAPHORE_CA;
            break;

			case CA_SIMPLE_JUNCTION:
			ca_type = SIMPLE_JUNCTION_CA;
			break;

            case NEW_CAR_PROB:
            new_car_prob = aux_string_to_num<double>(opt.arg);
            break;

			case NEW_CAR_SPEED:
			new_car_speed = aux_string_to_num<int>(opt.arg);
			break;

            case SMART_DENSITY:
            smart_density = aux_string_to_num<double>(opt.arg);
            break;

            case STOP_DENSITY:
            stop_density = aux_string_to_num<double>(opt.arg);
            break;

            case SEMAPHORE_DENSITY:
            semaphore_density = aux_string_to_num<double>(opt.arg);
            break;

            case RANDOM_SEMAPHORES:
            random_semaphores = true;
            break;

            case DT:
            dt = aux_string_to_num<double>(opt.arg);
            break;

            case DMIN:
            dmin = aux_string_to_num<double>(opt.arg);
            break;

            case DMAX:
            dmax = aux_string_to_num<double>(opt.arg);
            break;

            case VMAX_MIN:
            vmax_min = aux_string_to_num<int>(opt.arg);
            break;

            case VMAX_MAX:
            vmax_max = aux_string_to_num<int>(opt.arg);
            break;

            case SMART_MIN:
            smart_min = aux_string_to_num<double>(opt.arg);
            break;

            case SMART_MAX:
            smart_max = aux_string_to_num<double>(opt.arg);
            break;

            case STOP_DENSITY_MIN:
            stop_density_min = aux_string_to_num<double>(opt.arg);
            break;

            case STOP_DENSITY_MAX:
            stop_density_max = aux_string_to_num<double>(opt.arg);
            break;

            case SEMAPHORE_DENSITY_MIN:
            semaphore_density_min = aux_string_to_num<double>(opt.arg);
            break;

            case SEMAPHORE_DENSITY_MAX:
            semaphore_density_max = aux_string_to_num<double>(opt.arg);
            break;

            case RAND_PROB_MIN:
            rand_prob_min = aux_string_to_num<double>(opt.arg);
            break;

            case RAND_PROB_MAX:
            rand_prob_max = aux_string_to_num<double>(opt.arg);
            break;

            case NEW_CAR_MIN:
            new_car_min = aux_string_to_num<double>(opt.arg);
            break;

            case NEW_CAR_MAX:
            new_car_max = aux_string_to_num<double>(opt.arg);
            break;

            case OUT_FILE_NAME:
            out_file_name = opt.arg;
            break;

            case TEST:
            test = true;
            break;
        }
    }

    delete[] options;
    delete[] buffer;

    // Verifica opciones.
    if ((ocupancy_fixed || flow_fixed) && (flow_vs_density || flow_vs_vmax || flow_vs_rand_prob ||
        flow_vs_smart_cars || flow_vs_stop_density || flow_vs_new_car || flow_vs_semaphore_density))
    {
        cout << "Opciones incompatibles." << endl;
        return 1;
    }
    if (!(ocupancy_fixed || flow_fixed || flow_vs_density || flow_per_density || flow_vs_vmax 
        || flow_vs_rand_prob || flow_vs_smart_cars || flow_vs_new_car || flow_vs_stop_density || flow_vs_semaphore_density 
        || test))
    {
        plot_traffic = true;    // Opcion predeterminada.
    }
    if (flow_vs_vmax && dt < 1)
        dt = 1.0;

    double extra1 = 0.0;    // Parámetro extra en el constructor de CA.
	int extra2 = 0;
    bool extra3 = 0.0;
	if (ca_type == OPEN_CA || ca_type == SIMPLE_JUNCTION_CA)
	{
		extra1 = new_car_prob;
		extra2 = new_car_speed;
	}
    if (ca_type == SMART_CA)
        extra1 = smart_density;
    if (ca_type == STOP_CA)
        extra1 = stop_density;
    if (ca_type == SEMAPHORE_CA)
    {
        extra1 = semaphore_density;
        extra2 = random_semaphores;
    }

    // Realiza acciones.
    if (test)
    {
        cout << "Realizando tests." << endl;
        perform_test();
    }
    if (ocupancy_fixed || flow_fixed || plot_traffic)
    {
        cout << "Evolucionando automata." << endl;
		CellularAutomata* ca = create_ca(ca_type, size, density, vmax, rand_prob, 
			                             Args({ extra1 }, { extra2 }, { extra3 }));
        ca->Evolve(iterations);
        if (plot_traffic)
        {
            cout << "Creando mapas." << endl;
            ca->DrawHistory();
            ca->DrawFlowHistory();
        }
        if (ocupancy_fixed)
        {
            cout << "Midiendo ocupacion." << endl;
            ex_ocupancy_fixed(ca, out_file_name);
        }
        if (flow_fixed)
        {
            cout << "Midiendo flujo." << endl;
            ex_flow_fixed(ca, out_file_name);
        }
        delete_ca();
    }
    else
    {
        if (flow_vs_density)
        {
            cout << "Midiendo flujo vs densidad." << endl;
            ex_flow_vs_density(ca_type, size, iterations, vmax, dmin, dmax, dt, rand_prob, 
				               Args({ extra1 }, { extra2 }, { extra3 }), false, out_file_name);
        }
        if (flow_per_density)
        {
            cout << "Midiendo flujo/densidad vs densidad." << endl;
            ex_flow_vs_density(ca_type, size, iterations, vmax, dmin, dmax, dt, rand_prob, 
				               Args({ extra1 }, { extra2 }, { extra3 }), true, out_file_name);
        }
        if (flow_vs_vmax)
        {
            cout << "Midiendo flujo vs vmax." << endl;
            ex_flow_vs_vmax(ca_type, size, iterations, vmax_min, vmax_max, (int)dt, density, 
		               		rand_prob, Args({ extra1 }, { extra2 }, { extra3 }), out_file_name);
        }
        if (flow_vs_rand_prob)
        {
            cout << "Midiendo flujo vs rand_prob." << endl;
            ex_flow_vs_rand_prob(ca_type, size, iterations, vmax, density, rand_prob_min, 
				                 rand_prob_max, dt, Args({ extra1 }, { extra2 }, { extra3 }), 
								 out_file_name);
        }
        if (flow_vs_smart_cars)
        {
            cout << "Midiendo flujo vs densidad de autos inteligentes." << endl;
            ex_flow_vs_smart_cars(size, iterations, vmax, density, rand_prob, smart_min, 
                                  smart_max, dt, out_file_name);
        }
        if (flow_vs_stop_density)
        {
            cout << "Midiendo flujo vs densidad de topes." << endl;
            ex_flow_vs_stop_density(size, iterations, vmax, density, rand_prob, stop_density_min, 
                                    stop_density_max, dt, out_file_name);
        }
        if (flow_vs_new_car)
        {
            cout << "Midiendo flujo vs probabilidad de nuevo auto." << endl;
            ex_flow_vs_new_car_prob(size, iterations, vmax, density, rand_prob, new_car_min, 
				                    new_car_max, dt, new_car_speed, out_file_name);
        }
        if (flow_vs_semaphore_density)
        {
             cout << "Midiendo flujo vs densidad de semaforos." << endl;
             ex_flow_vs_semaphore_density(size, iterations, vmax, density, rand_prob, semaphore_density_min,
                                          semaphore_density_max, dt, random_semaphores, out_file_name);
        }
    }
    cout << "Hecho." << endl;
    return 0;
}