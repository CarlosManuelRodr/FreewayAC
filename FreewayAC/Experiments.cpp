#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "CellularAutomata.h"
#include "Experiments.h"
using namespace std;

/*****************************
*                            *
* Parámetros de experimentos *
*                            *
*****************************/

void ExParam::Report() const
{
    string out;
    out = "Parametros:\n";
    out += aux_report("size", size);
    out += aux_report("iterations", iterations);
    out += aux_report("lanes", lanes);
    out += aux_report("vmax", vmax);
    out += aux_report("vmax_min", vmax_min);
    out += aux_report("vmax_max", vmax_max);
    out += aux_report("random_seed", random_seed);
    out += aux_report("partitions", partitions);
    out += aux_report("density", density);
    out += aux_report("density_min", density_min);
    out += aux_report("density_max", density_max);
    out += aux_report("rand_prob", rand_prob);
    out += aux_report("rand_prob_min", rand_prob_min);
    out += aux_report("rand_prob_max", rand_prob_max);
    out += aux_report("init_vel", init_vel);
    out += aux_report("aut_car_density_min", aut_car_density_min);
    out += aux_report("aut_car_density_max", aut_car_density_max);
    out += aux_report("new_car_prob_min", new_car_prob_min);
    out += aux_report("new_car_prob_max", new_car_prob_max);
    out += aux_report("stop_density_min", stop_density_min);
    out += aux_report("stop_density_max", stop_density_max);
    out += aux_report("semaphore_density_min", semaphore_density_min);
    out += aux_report("semaphore_density_max", semaphore_density_max);
    out += aux_report("dt", dt);
    out += aux_report("random_semaphores", random_semaphores);
    out += aux_report("show_progress", show_progress);
    out += aux_report("path", path);
    out += aux_report("out_file_name", out_file_name);
    out += aux_report("threads", threads);

    cout << out;
    ofstream file;
    file.open(GetFilePath("Report.txt"), std::ofstream::out);
    file << out;
    file.close();
}
string ExParam::GetFilePath(string filename) const
{
    string out_file_path;
    if (out_file_name.empty())
        out_file_path = path + filename;
    else
        out_file_path = path + out_file_name;
    return out_file_path;
}


/****************************
*                           *
*          Medición         *
*                           *
****************************/

vector<Coord<double>> measure_permutation_entropy(vector<double> date, vector<double> svalues, int order, int length)
{
    // Basado en la rutina de David Hernández Enríquez que se encuentra en script/pentropy.py.
    if (date.size() != svalues.size())
    {
        cout << "Error: La longitudes de las listas no coinciden." << endl;
        return vector<Coord<double>>();
    }

    cout << "Orden: " << order << endl;
    cout << "Longitud: " << length << endl;
    vector<Coord<double>> out;
    vector<double> entropies, intervals;
    vector<double> selecf, selecdl, neighbors, permutation, perminv;

    int T, swap_num, pass, factorial;
    double H, p_value;

    int n = date.size();

    for (int rep = 0; rep < n - length + 1; ++rep)
    {
        selecf.assign(date.begin() + rep, date.begin() + length + rep);
        selecdl.assign(svalues.begin() + rep, svalues.begin() + length + rep);
        T = selecf.size() - order + 1;
        factorial = (int)ceil(tgamma(order + 1));
        H = 0;
        Matrix<double> vp(factorial, 3);
        vp.Assign(0.0);

        for (int i = 0; i < T; ++i)
        {
            neighbors.assign(selecdl.begin() + i, selecdl.begin() + i + order);
            permutation.assign(order, 0);
            vector<int> permu(order);
            iota(std::begin(permu), std::end(permu), 1);

            // Inicio de orderamiento burbuja.
            swap_num = 1;
            pass = 1;
            while (pass < (int)neighbors.size() && swap_num == 1)
            {
                swap_num = 0;
                for (int m = 0; m < (int)neighbors.size() - pass; ++m)
                {
                    if (neighbors[m] > neighbors[m + 1])
                    {
                        swap(neighbors[m], neighbors[m + 1]);
                        swap(permu[m], permu[m + 1]);
                        swap_num = 1;
                    }
                }
                ++pass;
            }

            // Fin orderamiento.
            for (int j = 0; j < order; ++j)
                permutation[permu[j] - 1] = j + 1;

            p_value = 0;
            perminv = permutation;
            reverse(perminv.begin(), perminv.end());

            // Ciclo para obtener el valor de la permutación.
            for (int j = 0; j < order; ++j)
                p_value += permutation[j] * pow(10.0, j);

            // Ciclo para acumular valores de permutationes repetidas.
            for (int l = 0; l < T; ++l)
            {
                if (p_value == vp[l][0])
                {
                    vp[l][1]++;
                    break;
                }
                else
                {
                    if (p_value != vp[l][0] && vp[l][0] == 0)
                    {
                        vp[l][0] = p_value;
                        vp[l][1]++;
                        break;
                    }
                }
            }
        }

        for (int j = 0; j < factorial; ++j)
        {
            if (vp[j][0] == 0)
                continue;
            vp[j][2] = vp[j][1] / (double)T;
            H -= vp[j][2] * log2(vp[j][2]);
        }
        out.push_back(Coord<double>(date[rep], H));
    }
    return out;
}

vector<Coord<double>> measure_permutation_entropy(vector<Coord<double>> data, int order, int length)
{
    return measure_permutation_entropy(aux_coordvec_to_vec(data, COORD_X), aux_coordvec_to_vec(data, COORD_Y), order, length);
}

/****************************
*                           *
*        Experimentos       *
*                           *
****************************/

int ex_traffic_map(ExParam p)
{
    CaHandler ca(p.type, p.size, p.lanes, p.density, p.vmax, p.rand_prob, p.init_vel, p.args);
    ca.Evolve(p.iterations);
    return ca.DrawHistory(p.path, p.out_file_name);
}

int ex_flow_map(ExParam p)
{
    CaHandler ca(p.type, p.size, p.lanes, p.density, p.vmax, p.rand_prob, p.init_vel, p.args);
    ca.Evolve(p.iterations);
    return ca.DrawFlowHistory(p.path, p.out_file_name);
}

int ex_ocupancy_fixed(ExParam p)
{
    CaHandler ca(p.type, p.size, p.lanes, p.density, p.vmax, p.rand_prob, p.init_vel, p.args);
    ca.Evolve(p.iterations);
    return export_data(ca.CalculateOcupancy(), p.GetFilePath("ocupancy.csv"), p.export_format);
}

int ex_flow_fixed(ExParam p)
{
    CaHandler ca(p.type, p.size, p.lanes, p.density, p.vmax, p.rand_prob, p.init_vel, p.args);
    ca.Evolve(p.iterations);
    return export_data(ca.CalculateFlow(), p.GetFilePath("flow.csv"), p.export_format);
}

Coord<double> ex_flow_vs_density_thread(double d, ExParam &p)
{
    // Evoluciona el sistema.
    CaHandler ca;
    ca.CreateCa(p.type, p.size, p.lanes, d, p.vmax, p.rand_prob, p.init_vel, p.args, p.random_seed);

    if (ca.Status() != 0)
        return Coord<double>(0.0, 0.0);
    ca.Evolve(p.iterations);

    // Obtiene el promedio de todos los flujos.
    double mean = ca.CalculateMeanFlow();
    if (p.per_density)
        mean /= d;

    return Coord<double>(d, mean);
}

int ex_flow_vs_density(ExParam p)
{
    vector<Coord<double>> dens_flow = aux_parallel_function<Coord<double>, double, ExParam&>(ex_flow_vs_density_thread, p.density_min, p.density_max, p.dt, p, p.threads);

    // Escribe a archivo.
    string filename;
    if (p.per_density)
        filename = "flow_per_density.csv";
    else
        filename = "flow_vs_density.csv";

    return export_data(dens_flow, p.GetFilePath(filename), p.export_format);
}

Coord<double> ex_flow_vs_vmax_thread(int v, ExParam &p)
{
    // Evoluciona el sistema.
    CaHandler ca;
    ca.CreateCa(p.type, p.size, p.lanes, p.density, v, p.rand_prob, p.init_vel, p.args, p.random_seed);

    if (ca.Status() != 0)
        return Coord<double>(0.0, 0.0);
    ca.Evolve(p.iterations);

    return Coord<double>(v, ca.CalculateMeanFlow());
}

int ex_flow_vs_vmax(ExParam p)
{
    vector<Coord<double>> vmax_flow = aux_parallel_function<Coord<double>, int, ExParam&>(ex_flow_vs_vmax_thread, p.vmax_min, p.vmax_max, (int)p.dt, p, p.threads);
    return export_data(vmax_flow, p.GetFilePath("flow_vs_vmax.csv"), p.export_format);
}

Coord<double> ex_flow_vs_rand_prob_thread(double r, ExParam &p)
{
    // Evoluciona el sistema.
    CaHandler ca;
    ca.CreateCa(p.type, p.size, p.lanes, p.density, p.vmax, r, p.init_vel, p.args, p.random_seed);

    if (ca.Status() != 0)
        return Coord<double>(0.0, 0.0);
    ca.Evolve(p.iterations);

    return Coord<double>(r, ca.CalculateMeanFlow());
}

int ex_flow_vs_rand_prob(ExParam p)
{
    vector<Coord<double>> rand_flow = aux_parallel_function<Coord<double>, double, ExParam&>(ex_flow_vs_rand_prob_thread, p.rand_prob_min, p.rand_prob_max, p.dt, p, p.threads);
    return export_data(rand_flow, p.GetFilePath("flow_vs_rand_prob.csv"), p.export_format);
}

Coord<double> ex_flow_vs_aut_cars_thread(double s, ExParam &p)
{
    // Evoluciona el sistema.
    CaHandler ca;
    ca.CreateCa(p.type, p.size, p.lanes, p.density, p.vmax, p.rand_prob, p.init_vel, Args({ s }), p.random_seed);

    if (ca.Status() != 0)
        return Coord<double>(0.0, 0.0);
    ca.Evolve(p.iterations);

    return Coord<double>(s, ca.CalculateMeanFlow());
}

int ex_flow_vs_aut_cars(ExParam p)
{
    if (!aux_is_in<CA_TYPE>({ AUTONOMOUS_CA, AUTONOMOUS_INSTONLY_CA, AUTONOMOUS_NORAND_CA }, p.type))
    {
        cout << "AC no valido para experimento seleccionado." << endl;
        return 1;
    }

    vector<Coord<double>> aut_flow = aux_parallel_function<Coord<double>, double, ExParam&>(ex_flow_vs_aut_cars_thread, p.aut_car_density_min, p.aut_car_density_max, p.dt, p, p.threads);
    return export_data(aut_flow, p.GetFilePath("flow_vs_aut_cars.csv"), p.export_format);
}

Coord<double> ex_flow_vs_new_car_prob_thread(double s, ExParam &p)
{
    // Evoluciona el sistema.
    Args temp = p.args;
    temp.SetDouble(0, s);
    CaHandler ca;
    ca.CreateCa(p.type, p.size, p.lanes, p.density, p.vmax, p.rand_prob, p.init_vel, temp, p.random_seed);

    if (ca.Status() != 0)
        return Coord<double>(0.0, 0.0);
    ca.Evolve(p.iterations);

    // Obtiene el promedio de todos los flujos.
    double mean = ca.CalculateMeanFlow();
    if (p.per_prob)
        mean /= s;

    return Coord<double>(s, mean);
}

int ex_flow_vs_new_car_prob(ExParam p)
{
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, p.type))
    {
        cout << "AC no valido para experimento seleccionado." << endl;
        return 1;
    }

    // Escribe a archivo.
    string filename;
    if (p.per_prob)
        filename = "flow_per_new_car_prob.csv";
    else
        filename = "flow_vs_new_car_prob.csv";

    vector<Coord<double>> new_car_flow = aux_parallel_function<Coord<double>, double, ExParam&>(ex_flow_vs_new_car_prob_thread, p.new_car_prob_min, p.new_car_prob_max, p.dt, p, p.threads);
    return export_data(new_car_flow, p.GetFilePath(filename), p.export_format);
}

Coord<double> ex_flow_vs_stop_density_thread(double d, ExParam &p)
{
    // Evoluciona el sistema.
    CaHandler ca;
    ca.CreateCa(STOP_CA, p.size, p.density, p.vmax, p.rand_prob, p.init_vel, Args({ d }), p.random_seed);

    if (ca.Status() != 0)
        return Coord<double>(0.0, 0.0);
    ca.Evolve(p.iterations);

    return Coord<double>(d, ca.CalculateMeanFlow());
}

int ex_flow_vs_stop_density(ExParam p)
{
    vector<Coord<double>> stop_flow = aux_parallel_function<Coord<double>, double, ExParam&>(ex_flow_vs_stop_density_thread, p.stop_density_min, p.stop_density_max, p.dt, p, p.threads);
    return export_data(stop_flow, p.GetFilePath("flow_vs_stop_density.csv"), p.export_format);
}

Coord<double> ex_flow_vs_semaphore_density_thread(double d, ExParam &p)
{
    // Evoluciona el sistema.
    CaHandler ca;
    ca.CreateCa(SEMAPHORE_CA, p.size, p.density, p.vmax, p.rand_prob, p.init_vel, Args({ d }, {}, { p.random_semaphores }), p.random_seed);

    if (ca.Status() != 0)
        return Coord<double>(0.0, 0.0);
    ca.Evolve(p.iterations);

    return Coord<double>(d, ca.CalculateMeanFlow());
}

int ex_flow_vs_semaphore_density(ExParam p)
{
    vector<Coord<double>> semaphore_flow = aux_parallel_function<Coord<double>, double, ExParam&>(ex_flow_vs_semaphore_density_thread, p.semaphore_density_min, p.semaphore_density_max, p.dt, p, p.threads);
    return export_data(semaphore_flow, p.GetFilePath("flow_vs_semaphore_density.csv"), p.export_format);
}

Coord<double> ex_escape_time_vs_density_thread(double d, ExParam &p)
{
    // Evoluciona el sistema.
    p.args.SetDouble(0, 0.0);
    CaHandler ca;
    ca.CreateCa(p.type, p.size, p.lanes, d, p.vmax, p.rand_prob, p.init_vel, p.args, p.random_seed);

    int iter = 0;
    while (ca.CountCars() != 0)
    {
        ca.Step();
        iter++;
    }
    if (iter != 0)
        return Coord<double>(d, (double)iter);
    else
        return Coord<double>(0.0, 0.0);
}

int ex_escape_time_vs_density(ExParam p)
{
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, OPEN_MULTILANE_CA, SIMPLE_JUNCTION_CA }, p.type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        p.type = OPEN_CA;
    }

    vector<Coord<double>> dens_escape = aux_parallel_function<Coord<double>, double, ExParam&>(ex_escape_time_vs_density_thread, p.density_min, p.density_max, p.dt, p, p.threads);
    return export_data(dens_escape, p.GetFilePath("escape_time_vs_density.csv"), p.export_format);
}

Coord<double> ex_escape_time_vs_rand_prob_thread(double ra, ExParam &p)
{
    // Evoluciona el sistema.
    p.args.SetDouble(0, 0.0);
    CaHandler ca;
    ca.CreateCa(p.type, p.size, p.lanes, p.density, p.vmax, ra, p.init_vel, p.args, p.random_seed);

    int iter = 0;
    while (ca.CountCars() != 0)
    {
        ca.Step();
        iter++;
    }
    if (iter != 0)
        return Coord<double>(ra, (double)iter);
    else
        return Coord<double>(0.0, 0.0);
}

int ex_escape_time_vs_rand_prob(ExParam p)
{
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, OPEN_MULTILANE_CA, SIMPLE_JUNCTION_CA }, p.type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        p.type = OPEN_CA;
    }

    vector<Coord<double>> rand_escape = aux_parallel_function<Coord<double>, double, ExParam&>(ex_escape_time_vs_rand_prob_thread, p.rand_prob_min, p.rand_prob_max, p.dt, p, p.threads);
    return export_data(rand_escape, p.GetFilePath("escape_time_vs_rand_prob.csv"), p.export_format);
}

Coord<double> ex_escape_time_vs_vmax_thread(int v, ExParam &p)
{
    // Evoluciona el sistema.
    p.args.SetDouble(0, 0.0);
    CaHandler ca;
    ca.CreateCa(p.type, p.size, p.lanes, p.density, v, p.rand_prob, p.init_vel, p.args, p.random_seed);

    int iter = 0;
    while (ca.CountCars() != 0)
    {
        ca.Step();
        iter++;
    }
    if (iter != 0)
        return Coord<double>((double)v, (double)iter);
    else
        return Coord<double>(0.0, 0.0);
}

int ex_escape_time_vs_vmax(ExParam p)
{
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, OPEN_MULTILANE_CA, SIMPLE_JUNCTION_CA }, p.type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        p.type = OPEN_CA;
    }

    vector<Coord<double>> vmax_escape = aux_parallel_function<Coord<double>, int, ExParam&>(ex_escape_time_vs_vmax_thread, p.vmax_min, p.vmax_max, (int)p.dt, p, p.threads);
    return export_data(vmax_escape, p.GetFilePath("escape_time_vs_vmax.csv"), p.export_format);
}

Coord<double> ex_discharge_vs_density_thread(double d, ExParam &p)
{
    // Evoluciona el sistema.
    p.args.SetDouble(0, 0.0);
    CaHandler ca;
    ca.CreateCa(p.type, p.size, p.lanes, d, p.vmax, p.rand_prob, p.init_vel, p.args, p.random_seed);

    int iter = 0;
    while (ca.CountCars() != 0)
    {
        ca.Step();
        iter++;
    }
    if (iter != 0)
        return Coord<double>(d, (double)(ca.GetSize())*d / (double)iter);
    else
        return Coord<double>(0.0, 0.0);
}

int ex_discharge_vs_density(ExParam p)
{
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, OPEN_MULTILANE_CA, SIMPLE_JUNCTION_CA }, p.type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        p.type = OPEN_CA;
    }

    vector<Coord<double>> dens_discharge = aux_parallel_function<Coord<double>, double, ExParam&>(ex_discharge_vs_density_thread, p.density_min, p.density_max, p.dt, p, p.threads);
    return export_data(dens_discharge, "discharge_vs_density.csv", p.export_format);
}


int ex_pentropy_vs_density(ExParam p)
{
    CA_TYPE ca_type = p.type;
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, OPEN_MULTILANE_CA, SIMPLE_JUNCTION_CA }, ca_type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        ca_type = OPEN_CA;
    }

    vector<Coord<double>> dens_discharge = aux_parallel_function<Coord<double>, double, ExParam&>(ex_discharge_vs_density_thread, p.density_min, p.density_max, p.dt, p, p.threads);
    int r = export_data(dens_discharge, "discharge_vs_density.csv", p.export_format);
    if (r != 0)
        return r;

    cout << "Midiendo entropia de permutacion." << endl;
    vector<Coord<double>> result = measure_permutation_entropy(dens_discharge, p.porder, p.pinterval);
    return export_data(result, p.GetFilePath("pentropy_vs_density.csv"), p.export_format);
}


/**
* @brief Realiza tests para verificar la corrección de los algoritmos.
*/
void ex_perform_test()
{
    // Circular.
    vector<int> init({1, -1, -1, 1, -1, 1, 1, -1, -1, 1, -1, -1, -1, 1, -1, -1, 1, -1, -1, -1});
    vector<int> end_circ({-1, 2, -1, 1, 0, -1, -1, 1, -1, 1, -1,-1, -1, -1, 2, -1, -1, -1, 2, -1});
    vector<bool> rand_val_circ({ false, false, false, false, true, false, true,
                                 true, false, false, true, false, false, false,
                                 false, true, false, false, true, false, false });

    cout << "Comprobando automata celular circular... ";
    CircularCA circ_ca(init, rand_val_circ, 5);
    circ_ca.Evolve(3);

    bool match = true;
    for (unsigned i=0; i<init.size(); i++)
    {
        if (circ_ca.GetAt(i) != end_circ[i])
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
    vector<int> end_open({-1, 1, 0, -1, -1, 1, -1, 1, 0, -1, -1,-1, -1, -1, -1, 3, -1, -1, 2, -1});
    vector<bool> rand_val_open_vec({ false, false, false, false, true, false, true, false,
                                     true, false, false, true, false, false, false, true,
                                     false, true, false, false, true, false, false, false });

    cout << "Comprobando automata celular abierto... ";
    OpenCA open_ca(init, rand_val_open_vec, 5, 1);
    open_ca.Evolve(3);

    match = true;
    for (unsigned i=0; i<init.size(); i++)
    {
        if (open_ca.GetAt(i) != end_open[i])
            match = false;
    }

    if (match)
        cout << "Correcto!" << endl;
    else
    {
        cout << "Incorrecto. Log:" << endl;
        open_ca.PrintHistory();
    }

    // Inteligente.
    vector<int> end_aut({ -1, -1, -1, -1, 1, 0, -1, 1, -1, -1, -1, -1, 2, -1, -1, 2, -1, 1, -1, 1 });
    vector<int> aut_cars_vec({ 0, 1, 3, 5 });
    vector<bool> rand_val_aut_vec({ false, false, false, true, true,
                                    true, false, true, false, false,
                                    false, true, false, true, false });

    cout << "Comprobando automata celular con vehiculos autonomos... ";
    AutonomousCA autonomous_ca(init, aut_cars_vec, rand_val_aut_vec, 5);
    autonomous_ca.Evolve(3);

    match = true;
    for (unsigned i = 0; i<init.size(); i++)
    {
        if (autonomous_ca.GetAt(i) != end_aut[i])
            match = false;
    }

    if (match)
        cout << "Correcto!" << endl;
    else
    {
        cout << "Incorrecto. Log:" << endl;
        autonomous_ca.PrintHistory();
    }
}
