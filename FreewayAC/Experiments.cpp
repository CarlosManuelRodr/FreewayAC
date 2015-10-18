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

void ex_traffic_map(ExParam p)
{
    CaHandler ca(p.type, p.size, p.lanes, p.density, p.vmax, p.rand_prob, p.init_vel, p.args);
    ca.Evolve(p.iterations);
    ca.DrawHistory(p.path, p.out_file_name);
    ca.PrintHistoryToFile(p.path, p.out_file_name);
}

void ex_flow_map(ExParam p)
{
    CaHandler ca(p.type, p.size, p.lanes, p.density, p.vmax, p.rand_prob, p.init_vel, p.args);
    ca.Evolve(p.iterations);
    ca.DrawFlowHistory(p.path, p.out_file_name);
}

void ex_ocupancy_fixed(ExParam p)
{
    CaHandler ca(p.type, p.size, p.lanes, p.density, p.vmax, p.rand_prob, p.init_vel, p.args);
    ca.Evolve(p.iterations);
    export_data(ca.CalculateOcupancy(), p.GetFilePath("ocupancy.csv"), p.export_format);
}

void ex_flow_fixed(ExParam p)
{
    CaHandler ca(p.type, p.size, p.lanes, p.density, p.vmax, p.rand_prob, p.init_vel, p.args);
    ca.Evolve(p.iterations);
    export_data(ca.CalculateFlow(), p.GetFilePath("flow.csv"), p.export_format);
}

Coord<double> ex_flow_vs_density_thread(double d, ExParam &p)
{
    // Evoluciona el sistema.
    CaHandler ca(p.type, p.size, p.lanes, d, p.vmax, p.rand_prob, p.init_vel, p.args, p.random_seed);
    ca.Evolve(p.iterations);

    // Obtiene el promedio de todos los flujos.
    double mean = ca.CalculateMeanFlow();
    if (p.per_density)
        mean /= d;

    return Coord<double>(d, mean);
}

void ex_flow_vs_density(ExParam p)
{
    vector<Coord<double>> dens_flow = aux_parallel_function<Coord<double>, double, ExParam&>(ex_flow_vs_density_thread, p.density_min, p.density_max, p.dt, p, p.threads);

    // Escribe a archivo.
    string filename;
    if (p.per_density)
        filename = "flow_per_density.csv";
    else
        filename = "flow_vs_density.csv";

    export_data(dens_flow, p.GetFilePath(filename), p.export_format);
}

Coord<double> ex_flow_vs_vmax_thread(int v, ExParam &p)
{
    // Evoluciona el sistema.
    CaHandler ca(p.type, p.size, p.lanes, p.density, v, p.rand_prob, p.init_vel, p.args, p.random_seed);
    ca.Evolve(p.iterations);
    return Coord<double>(v, ca.CalculateMeanFlow());
}

void ex_flow_vs_vmax(ExParam p)
{
    vector<Coord<double>> vmax_flow = aux_parallel_function<Coord<double>, int, ExParam&>(ex_flow_vs_vmax_thread, p.vmax_min, p.vmax_max, (int)p.dt, p, p.threads);
    export_data(vmax_flow, p.GetFilePath("flow_vs_vmax.csv"), p.export_format);
}

Coord<double> ex_flow_vs_rand_prob_thread(double r, ExParam &p)
{
    // Evoluciona el sistema.
    CaHandler ca(p.type, p.size, p.lanes, p.density, p.vmax, r, p.init_vel, p.args, p.random_seed);
    ca.Evolve(p.iterations);
    return Coord<double>(r, ca.CalculateMeanFlow());
}

void ex_flow_vs_rand_prob(ExParam p)
{
    vector<Coord<double>> rand_flow = aux_parallel_function<Coord<double>, double, ExParam&>(ex_flow_vs_rand_prob_thread, p.rand_prob_min, p.rand_prob_max, p.dt, p, p.threads);
    export_data(rand_flow, p.GetFilePath("flow_vs_rand_prob.csv"), p.export_format);
}

Coord<double> ex_flow_vs_aut_cars_thread(double s, ExParam &p)
{
    // Evoluciona el sistema.
    CaHandler ca(p.type, p.size, p.lanes, p.density, p.vmax, p.rand_prob, p.init_vel, Args({ s }), p.random_seed);
    ca.Evolve(p.iterations);
    return Coord<double>(s, ca.CalculateMeanFlow());
}

void ex_flow_vs_aut_cars(ExParam p)
{
    if (!aux_is_in<CA_TYPE>({ AUTONOMOUS_CA, AUTONOMOUS_INSTONLY_CA, AUTONOMOUS_NORAND_CA }, p.type))
        throw CaArgumentError("ex_flow_vs_aut_cars: AC no valido para experimento seleccionado.");

    vector<Coord<double>> aut_flow = aux_parallel_function<Coord<double>, double, ExParam&>(ex_flow_vs_aut_cars_thread, p.aut_car_density_min, p.aut_car_density_max, p.dt, p, p.threads);
    export_data(aut_flow, p.GetFilePath("flow_vs_aut_cars.csv"), p.export_format);
}

Coord<double> ex_flow_vs_new_car_prob_thread(double s, ExParam &p)
{
    // Evoluciona el sistema.
    Args temp = p.args;
    temp.SetDouble(0, s);
    CaHandler ca(p.type, p.size, p.lanes, p.density, p.vmax, p.rand_prob, p.init_vel, temp, p.random_seed);
    ca.Evolve(p.iterations);

    // Obtiene el promedio de todos los flujos.
    double mean = ca.CalculateMeanFlow();
    if (p.per_prob)
        mean /= s;

    return Coord<double>(s, mean);
}

void ex_flow_vs_new_car_prob(ExParam p)
{
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, p.type))
        throw CaArgumentError("ex_flow_vs_new_car_prob: AC no valido para experimento seleccionado.");

    // Escribe a archivo.
    string filename;
    if (p.per_prob)
        filename = "flow_per_new_car_prob.csv";
    else
        filename = "flow_vs_new_car_prob.csv";

    vector<Coord<double>> new_car_flow = aux_parallel_function<Coord<double>, double, ExParam&>(ex_flow_vs_new_car_prob_thread, p.new_car_prob_min, p.new_car_prob_max, p.dt, p, p.threads);
    export_data(new_car_flow, p.GetFilePath(filename), p.export_format);
}

Coord<double> ex_flow_vs_stop_density_thread(double d, ExParam &p)
{
    // Evoluciona el sistema.
    CaHandler ca(STOP_CA, p.size, p.density, p.vmax, p.rand_prob, p.init_vel, Args({ d }), p.random_seed);
    ca.Evolve(p.iterations);
    return Coord<double>(d, ca.CalculateMeanFlow());
}

void ex_flow_vs_stop_density(ExParam p)
{
    vector<Coord<double>> stop_flow = aux_parallel_function<Coord<double>, double, ExParam&>(ex_flow_vs_stop_density_thread, p.stop_density_min, p.stop_density_max, p.dt, p, p.threads);
    export_data(stop_flow, p.GetFilePath("flow_vs_stop_density.csv"), p.export_format);
}

Coord<double> ex_flow_vs_semaphore_density_thread(double d, ExParam &p)
{
    // Evoluciona el sistema.
    CaHandler ca(SEMAPHORE_CA, p.size, p.density, p.vmax, p.rand_prob, p.init_vel, Args({ d }, {}, { p.random_semaphores }), p.random_seed);
    ca.Evolve(p.iterations);
    return Coord<double>(d, ca.CalculateMeanFlow());
}

void ex_flow_vs_semaphore_density(ExParam p)
{
    vector<Coord<double>> semaphore_flow = aux_parallel_function<Coord<double>, double, ExParam&>(ex_flow_vs_semaphore_density_thread,p.semaphore_density_min, 
            p.semaphore_density_max, p.dt, p, p.threads);
    export_data(semaphore_flow, p.GetFilePath("flow_vs_semaphore_density.csv"), p.export_format);
}

Coord<double> ex_escape_time_vs_density_thread(double d, ExParam &p)
{
    // Evoluciona el sistema.
    p.args.SetDouble(0, 0.0);
    CaHandler ca(p.type, p.size, p.lanes, d, p.vmax, p.rand_prob, p.init_vel, p.args, p.random_seed);

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

void ex_escape_time_vs_density(ExParam p)
{
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, OPEN_MULTILANE_CA, SIMPLE_JUNCTION_CA }, p.type))
        throw CaArgumentError("ex_escape_time_vs_density: AC no valido para experimento seleccionado.");

    vector<Coord<double>> dens_escape = aux_parallel_function<Coord<double>, double, ExParam&>(ex_escape_time_vs_density_thread, p.density_min, p.density_max, p.dt, p, p.threads);
    export_data(dens_escape, p.GetFilePath("escape_time_vs_density.csv"), p.export_format);
}

Coord<double> ex_escape_time_vs_rand_prob_thread(double ra, ExParam &p)
{
    // Evoluciona el sistema.
    p.args.SetDouble(0, 0.0);
    CaHandler ca(p.type, p.size, p.lanes, p.density, p.vmax, ra, p.init_vel, p.args, p.random_seed);

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

void ex_escape_time_vs_rand_prob(ExParam p)
{
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, OPEN_MULTILANE_CA, SIMPLE_JUNCTION_CA }, p.type))
        throw CaArgumentError("ex_escape_time_vs_rand_prob: AC no valido para experimento seleccionado.");

    vector<Coord<double>> rand_escape = aux_parallel_function<Coord<double>, double, ExParam&>(ex_escape_time_vs_rand_prob_thread, p.rand_prob_min, p.rand_prob_max, p.dt, p, p.threads);
    export_data(rand_escape, p.GetFilePath("escape_time_vs_rand_prob.csv"), p.export_format);
}

Coord<double> ex_escape_time_vs_vmax_thread(int v, ExParam &p)
{
    // Evoluciona el sistema.
    p.args.SetDouble(0, 0.0);
    CaHandler ca(p.type, p.size, p.lanes, p.density, v, p.rand_prob, p.init_vel, p.args, p.random_seed);

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

void ex_escape_time_vs_vmax(ExParam p)
{
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, OPEN_MULTILANE_CA, SIMPLE_JUNCTION_CA }, p.type))
        throw CaArgumentError("ex_escape_time_vs_vmax: AC no valido para experimento seleccionado.");

    vector<Coord<double>> vmax_escape = aux_parallel_function<Coord<double>, int, ExParam&>(ex_escape_time_vs_vmax_thread, p.vmax_min, p.vmax_max, (int)p.dt, p, p.threads);
    export_data(vmax_escape, p.GetFilePath("escape_time_vs_vmax.csv"), p.export_format);
}

Coord<double> ex_discharge_vs_density_thread(double d, ExParam &p)
{
    // Evoluciona el sistema.
    p.args.SetDouble(0, 0.0);
    CaHandler ca(p.type, p.size, p.lanes, d, p.vmax, p.rand_prob, p.init_vel, p.args, p.random_seed);

    int iter = 0;
    while (ca.CountCars() != 0)
    {
        ca.Step();
        iter++;
    }
    if (iter != 0)
        return Coord<double>(d, (double)(ca.GetSize()*ca.GetLanes())*d / (double)iter);
    else
        return Coord<double>(0.0, 0.0);
}

void ex_discharge_vs_density(ExParam p)
{
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, OPEN_MULTILANE_CA, SIMPLE_JUNCTION_CA }, p.type))
        throw CaArgumentError("ex_discharge_vs_density: AC no valido para experimento seleccionado.");

    vector<Coord<double>> dens_discharge = aux_parallel_function<Coord<double>, double, ExParam&>(ex_discharge_vs_density_thread, p.density_min, p.density_max, p.dt, p, p.threads);
    export_data(dens_discharge, "discharge_vs_density.csv", p.export_format);
}


void ex_pentropy_vs_density(ExParam p)
{
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, OPEN_MULTILANE_CA, SIMPLE_JUNCTION_CA }, p.type))
        throw CaArgumentError("ex_pentropy_vs_density: AC no valido para experimento seleccionado.");

    vector<Coord<double>> dens_discharge = aux_parallel_function<Coord<double>, double, ExParam&>(ex_discharge_vs_density_thread, p.density_min, p.density_max, p.dt, p, p.threads);
    export_data(dens_discharge, "discharge_vs_density.csv", p.export_format);

    cout << "Midiendo entropia de permutacion." << endl;
    vector<Coord<double>> result = measure_permutation_entropy(dens_discharge, p.porder, p.pinterval);
    export_data(result, p.GetFilePath("pentropy_vs_density.csv"), p.export_format);
}