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

void ExParam::Report()
{
    cout << "Parametros:" << endl;
    aux_report("size", size);
    aux_report("iterations", iterations);
    aux_report("lanes", lanes);
    aux_report("vmax", vmax);
    aux_report("vmax_min", vmax_min);
    aux_report("vmax_max", vmax_max);
    aux_report("random_seed", random_seed);
    aux_report("partitions", partitions);
    aux_report("density", density);
    aux_report("density_min", density_min);
    aux_report("density_max", density_max);
    aux_report("rand_prob", rand_prob);
    aux_report("rand_prob_min", rand_prob_min);
    aux_report("rand_prob_max", rand_prob_max);
    aux_report("init_vel", init_vel);
    aux_report("aut_car_density_min", aut_car_density_min);
    aux_report("aut_car_density_max", aut_car_density_max);
    aux_report("new_car_prob_min", new_car_prob_min);
    aux_report("new_car_prob_max", new_car_prob_max);
    aux_report("stop_density_min", stop_density_min);
    aux_report("stop_density_max", stop_density_max);
    aux_report("semaphore_density_min", semaphore_density_min);
    aux_report("semaphore_density_max", semaphore_density_max);
    aux_report("dt", dt);
    aux_report("random_semaphores", random_semaphores);
    aux_report("show_progress", show_progress);
    aux_report("path", path);
    aux_report("out_file_name", out_file_name);
}
string ExParam::GetFilePath(string filename)
{
    if (out_file_name.empty())
        out_file_name = path + filename;
    else
        out_file_name = path + out_file_name;
    return out_file_name;
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

    for (int rep = 0; rep < n - length + 1; rep++)
    {
        selecf.assign(date.begin() + rep, date.begin() + length + rep);
        selecdl.assign(svalues.begin() + rep, svalues.begin() + length + rep);
        T = selecf.size() - order + 1;
        factorial = (int)ceil(tgamma(order + 1));
        H = 0;
        Matrix<double> vp(factorial, 3);
        vp.Assign(0.0);

        for (int i = 0; i < T; i++)
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
                for (int m = 0; m < (int)neighbors.size() - pass; m++)
                {
                    if (neighbors[m] > neighbors[m + 1])
                    {
                        swap(neighbors[m], neighbors[m + 1]);
                        swap(permu[m], permu[m + 1]);
                        swap_num = 1;
                    }
                }
                pass++;
            }

            // Fin orderamiento.
            for (int j = 0; j < order; j++)
                permutation[permu[j] - 1] = j + 1;

            p_value = 0;
            perminv = permutation;
            reverse(perminv.begin(), perminv.end());

            // Ciclo para obtener el valor de la permutación.
            for (int j = 0; j < order; j++)
                p_value += permutation[j] * pow(10.0, j);

            // Ciclo para acumular valores de permutationes repetidas.
            for (int l = 0; l < T; l++)
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

        for (int j = 0; j < factorial; j++)
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

/****************************
*                           *
*        Experimentos       *
*                           *
****************************/

int ex_traffic_map(ExParam p)
{
    CaHandler ca(p.type, p.size, p.density, p.vmax, p.rand_prob, p.init_vel, p.args);
    ca.Evolve(p.iterations);
    return ca.DrawHistory(p.path, p.out_file_name);
}

int ex_flow_map(ExParam p)
{
    CaHandler ca(p.type, p.size, p.density, p.vmax, p.rand_prob, p.init_vel, p.args);
    ca.Evolve(p.iterations);
    return ca.DrawFlowHistory(p.path, p.out_file_name);
}

int ex_ocupancy_fixed(ExParam p)
{
    CaHandler ca(p.type, p.size, p.density, p.vmax, p.rand_prob, p.init_vel, p.args);
    ca.Evolve(p.iterations);
    return export_data(ca.CalculateOcupancy(), p.GetFilePath("ocupancy.csv"), p.export_format);
}

int ex_flow_fixed(ExParam p)
{
    CaHandler ca(p.type, p.size, p.density, p.vmax, p.rand_prob, p.init_vel, p.args);
    ca.Evolve(p.iterations);
    return export_data(ca.CalculateFlow(), p.GetFilePath("flow.csv"), p.export_format);
}

int ex_flow_vs_density(ExParam p)
{
    vector<double> density, flow;
    CaHandler ca;

    for (double d = p.density_min; d <= p.density_max; d += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(d, p.density_min, p.density_max, p.dt);

        // Evoluciona el sistema.
        ca.CreateCa(p.type, p.size, d, p.vmax, p.rand_prob, p.init_vel, p.args, p.random_seed);
        if (ca.Status() != 0)
            return 1;
        ca.Evolve(p.iterations);

        // Obtiene el promedio de todos los flujos.
        double mean = ca.CalculateMeanFlow();
        if (p.per_density) mean /= d;

        // Asigna valores.
        density.push_back(d);
        flow.push_back(mean);
    }

    // Escribe a archivo.
    string filename;
    if (p.per_density)
        filename = "flow_per_density.csv";
    else
        filename =  "flow_vs_density.csv";

    return export_data(density, flow, p.GetFilePath(filename), p.export_format);
}

int ex_flow_vs_vmax(ExParam p)
{
    vector<double> vmax, flow;
    CaHandler ca;

    for (int v = p.vmax_min; v <= p.vmax_max; v += (int)p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(v, p.vmax_min, p.vmax_max, p.dt);

        // Evoluciona el sistema.
        ca.CreateCa(p.type, p.size, p.density, v, p.rand_prob, p.init_vel, p.args, p.random_seed);
        if (ca.Status() != 0)
            return 1;
        ca.Evolve(p.iterations);

        // Asigna valores.
        vmax.push_back(v);
        flow.push_back(ca.CalculateMeanFlow());
    }

    return export_data(vmax, flow, p.GetFilePath("flow_vs_vmax.csv"), p.export_format);
}

int ex_flow_vs_rand_prob(ExParam p)
{
    vector<double> rand_prob, flow;
    CaHandler ca;

    for (double r = p.rand_prob_min; r <= p.rand_prob_max; r += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(r, p.rand_prob_min, p.rand_prob_max, p.dt);

        // Evoluciona el sistema.
        ca.CreateCa(p.type, p.size, p.density, p.vmax, r, p.init_vel, p.args, p.random_seed);
        if (ca.Status() != 0)
            return 1;
        ca.Evolve(p.iterations);

        // Asigna valores.
        rand_prob.push_back(r);
        flow.push_back(ca.CalculateMeanFlow());
    }

    return export_data(rand_prob, flow, p.GetFilePath("flow_vs_rand_prob.csv"), p.export_format);
}

int ex_flow_vs_aut_cars(ExParam p)
{
    vector<double> aut_car_density, flow;
    CaHandler ca;

    for (double s = p.aut_car_density_min; s <= p.aut_car_density_max; s += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(s, p.aut_car_density_min, p.aut_car_density_max, p.dt);

        // Evoluciona el sistema.
        ca.CreateCa(AUTONOMOUS_CA, p.size, p.density, p.vmax, p.rand_prob, p.init_vel, Args({ s }), p.random_seed);
        if (ca.Status() != 0)
            return 1;
        ca.Evolve(p.iterations);

        // Asigna valores.
        aut_car_density.push_back(s);
        flow.push_back(ca.CalculateMeanFlow());
    }

    return export_data(aut_car_density, flow, p.GetFilePath("flow_vs_aut_cars.csv"), p.export_format);
}

int ex_flow_vs_new_car_prob(ExParam p)
{
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, p.type))
    {
        cout << "AC no valido para experimento seleccionado." << endl;
        return 1;
    }

    vector<double> new_car_density, flow;
    CaHandler ca;

    for (double s = p.new_car_prob_min; s <= p.new_car_prob_max; s += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(s, p.new_car_prob_min, p.new_car_prob_max, p.dt);

        // Evoluciona el sistema.
        p.args.SetDouble(0, s);
        ca.CreateCa(p.type, p.size, p.density, p.vmax, p.rand_prob, p.init_vel, p.args, p.random_seed);
        if (ca.Status() != 0)
            return 1;
        ca.Evolve(p.iterations);

        // Obtiene el promedio de todos los flujos.
        double mean = ca.CalculateMeanFlow();
        if (p.per_prob)
            mean /= s;

        // Asigna valores.
        new_car_density.push_back(s);
        flow.push_back(mean);
    }

    // Escribe a archivo.
    string filename;
    if (p.per_prob)
        filename = "flow_per_new_car_prob.csv";
    else
        filename = "flow_vs_new_car_prob.csv";

    return export_data(new_car_density, flow, p.GetFilePath(filename), p.export_format);
}

int ex_flow_vs_stop_density(ExParam p)
{
    vector<double> stop_density, flow;
    CaHandler ca;

    for (double d = p.stop_density_min; d <= p.stop_density_max; d += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(d, p.stop_density_min, p.stop_density_max, p.dt);

        // Evoluciona el sistema.
        ca.CreateCa(STOP_CA, p.size, p.density, p.vmax, p.rand_prob, p.init_vel, Args({ d }), p.random_seed);
        if (ca.Status() != 0)
            return 1;
        ca.Evolve(p.iterations);

        // Asigna valores.
        stop_density.push_back(d);
        flow.push_back(ca.CalculateMeanFlow());
    }

    return export_data(stop_density, flow, p.GetFilePath("flow_vs_stop_density.csv"), p.export_format);
}

int ex_flow_vs_semaphore_density(ExParam p)
{
    vector<double> semaphore_density, flow;
    CaHandler ca;

    for (double d = p.semaphore_density_min; d <= p.semaphore_density_max; d += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(d, p.semaphore_density_min, p.semaphore_density_max, p.dt);

        // Evoluciona el sistema.
        ca.CreateCa(SEMAPHORE_CA, p.size, p.density, p.vmax, p.rand_prob, p.init_vel, Args({ d }, {}, { p.random_semaphores }), p.random_seed);
        if (ca.Status() != 0)
            return 1;
        ca.Evolve(p.iterations);

        // Asigna valores.
        semaphore_density.push_back(d);
        flow.push_back(ca.CalculateMeanFlow());
    }

    return export_data(semaphore_density, flow, p.GetFilePath("flow_vs_semaphore_density.csv"), p.export_format);
}

int ex_escape_time_vs_density(ExParam p)
{
    CA_TYPE ca_type = p.type;
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, ca_type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        ca_type = OPEN_CA;
    }

    vector<double> densities, escape_time;
    CaHandler ca;

    for (double d = p.density_min; d <= p.density_max; d += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(d, p.density_min, p.density_max, p.dt);

        // Evoluciona el sistema.
        p.args.SetDouble(0, 0.0);
        ca.CreateCa(ca_type, p.size, d, p.vmax, p.rand_prob, p.init_vel, p.args, p.random_seed);
        if (ca.Status() != 0)
            return 1;

        int iter = 0;
        while (ca.CountCars() != 0)
        {
            ca.Step();
            iter++;

            if (ca.IsFluxHalted())
            {
                cout << "Error: Flujo detenido." << endl;
                return 1;
            }
        }
        densities.push_back(d);
        escape_time.push_back(iter);

    }
    return export_data(densities, escape_time, p.GetFilePath("escape_time_vs_density.csv"), p.export_format);
}

int ex_escape_time_vs_rand_prob(ExParam p)
{
    CA_TYPE ca_type = p.type;
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, ca_type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        ca_type = OPEN_CA;
    }

    vector<double> rand_p, escape_time;
    CaHandler ca;

    for (double ra = p.rand_prob_min; ra <= p.rand_prob_max; ra += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(ra, p.rand_prob_min, p.rand_prob_max, p.dt);

        // Evoluciona el sistema.
        p.args.SetDouble(0, 0.0);
        ca.CreateCa(ca_type, p.size, p.density, p.vmax, ra, p.init_vel, p.args, p.random_seed);
        if (ca.Status() != 0)
            return 1;

        int iter = 0;
        while (ca.CountCars() != 0 && !ca.IsFluxHalted())
        {
            ca.Step();
            iter++;
        }
        rand_p.push_back(ra);
        escape_time.push_back(iter);

    }
    return export_data(rand_p, escape_time, p.GetFilePath("escape_time_vs_rand_prob.csv"), p.export_format);
}

int ex_escape_time_vs_vmax(ExParam p)
{
    CA_TYPE ca_type = p.type;
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, ca_type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        ca_type = OPEN_CA;
    }

    vector<double> vel, escape_time;
    CaHandler ca;

    for (int v = p.vmax_min; v <= p.vmax_max; v += (int)p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(v, p.vmax_min, p.vmax_max, p.dt);

        // Evoluciona el sistema.
        p.args.SetDouble(0, 0.0);
        ca.CreateCa(ca_type, p.size, p.density, v, p.rand_prob, p.init_vel, p.args, p.random_seed);
        if (ca.Status() != 0)
            return 1;

        int iter = 0;
        while (ca.CountCars() != 0 && !ca.IsFluxHalted())
        {
            ca.Step();
            iter++;
        }
        vel.push_back(v);
        escape_time.push_back(iter);

    }
    return export_data(vel, escape_time, p.GetFilePath("escape_time_vs_vmax.csv"), p.export_format);
}

int ex_discharge_vs_density(ExParam p)
{
    CA_TYPE ca_type = p.type;
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, ca_type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        ca_type = OPEN_CA;
    }

    vector<double> density, discharge;
    CaHandler ca;

    for (double d = p.density_min; d <= p.density_max; d += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(d, p.density_min, p.density_max, p.dt);

        if (d == 0)
            continue;

        // Evoluciona el sistema.
        p.args.SetDouble(0, 0.0);
        ca.CreateCa(ca_type, p.size, d, p.vmax, p.rand_prob, p.init_vel, p.args, p.random_seed);
        if (ca.Status() != 0)
            return 1;

        int iter = 0;
        while (ca.CountCars() != 0)
        {
            ca.Step();
            iter++;
        }
        if (iter != 0)
        {
            density.push_back(d);
            discharge.push_back((double)(ca.GetSize())*d / (double)iter);
        }

    }
    return export_data(density, discharge, p.GetFilePath("discharge_vs_density.csv"), p.export_format);
}

int ex_discharge_vs_density_fratal(ExParam p)
{
    CA_TYPE ca_type = p.type;
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, ca_type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        ca_type = OPEN_CA;
    }

    vector<double> escape_time;
    CaHandler ca;

    for (double d = p.density_min; d <= p.density_max; d += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(d, p.density_min, p.density_max, p.dt);

        // Evoluciona el sistema.
        p.args.SetDouble(0, 0.0);
        ca.CreateCa(ca_type, p.size, d, p.vmax, p.rand_prob, p.init_vel, p.args, p.random_seed);
        if (ca.Status() != 0)
            return 1;

        int iter = 0;
        while (ca.CountCars() != 0)
        {
            ca.Step();
            iter++;

            if (ca.IsFluxHalted())
            {
                cout << "Error: Flujo detenido." << endl;
                return 1;
            }
        }
        if (iter != 0)
            escape_time.push_back((double)(ca.GetSize())*d / (double)iter);
    }

    cout << "Calculando dimension fractal." << endl;
    double et_mean = aux_mean(escape_time);
    vector<int> fractal(escape_time.size(), 0);
    for (unsigned i = 0; i < escape_time.size(); ++i)
    {
        if (escape_time[i] > et_mean)
            fractal[i] = 1;
    }

    cout << "La dimension fractal es: ";
    cout << measure_plot_fractal_dimension(escape_time, (int)(0.1*fractal.size()), fractal.size(), 1);
    cout << "." << endl;

    cout << "La dimension fractal del grafico de barras es: ";
    cout << measure_fractal_dimension(fractal, 0, (int)(0.1*fractal.size()), fractal.size(), 1);
    cout << "." << endl;
    return export_map(fractal, p.GetFilePath("discharge_vs_density_fractal.bmp"), 30);
}

int ex_dimension_vs_density(ExParam p)
{
    int r;
    CA_TYPE ca_type = p.type;
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, ca_type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        ca_type = OPEN_CA;
    }

    // Crea directorios de salida para gráficas.
    aux_create_directory("Fractal");
    aux_create_directory("Plot");
    aux_create_directory("Csv");
    string f_path = p.path + "Fractal" + df_separator;
    string f_plot_path = p.path + "Plot" + df_separator;
    string csv_path = p.path + "Csv" + df_separator;

    vector<double> densities, dimension, plot_dimension;
    double p_size = (p.density_max - p.density_min) / (double)p.partitions;
    CaHandler ca;

    int part = 0;
    for (double d_left = p.density_min; part < p.partitions; d_left += p_size, part++)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(part, 0, p.partitions - 1, 1);

        vector<double> escape_time;
        for (double d = d_left; d < (d_left + p_size); d += p.dt)
        {
            // Evoluciona el sistema.
            p.args.SetDouble(0, 0.0);
            ca.CreateCa(ca_type, p.size, d, p.vmax, p.rand_prob, p.init_vel, p.args, p.random_seed);
            if (ca.Status() != 0)
                return 1;

            int iter = 0;
            while (ca.CountCars() != 0)
            {
                ca.Step();
                iter++;

                if (ca.IsFluxHalted())
                {
                    cout << "Error: Flujo detenido." << endl;
                    return 1;
                }
            }
            if (iter != 0)
                escape_time.push_back((double)(ca.GetSize())*d / (double)iter);
        }

        // Obtiene la media y selecciona los puntos mayores e inferiores
        // para generar fractal.
        double et_mean = aux_mean(escape_time);
        vector<int> fractal(escape_time.size(), 0);
        for (unsigned i = 0; i < escape_time.size(); ++i)
        {
            if (escape_time[i] > et_mean)
                fractal[i] = 1;
        }

        // Calcula dimensión fractal.
        double d_mean = d_left + p_size / 2.0;
        densities.push_back(d_mean);
        dimension.push_back(measure_fractal_dimension(fractal, 0, (int)(0.1*fractal.size()), fractal.size(), 1));
        plot_dimension.push_back(measure_plot_fractal_dimension(escape_time, (int)(0.1*fractal.size()), fractal.size(), 1));

        // Exporta los datos.
        string p_name = f_path + "discharge_vs_density_fractal_" + to_string(d_mean) + ".bmp";
        string p_plot_name = f_plot_path + "discharge_vs_density_plot_" + to_string(d_mean) + ".bmp";
        string csv_name = csv_path + "discharge_vs_density_" + to_string(d_mean) + ".csv";
        r = export_map(fractal, p_name, 30);
        if (r != 0)
            return r;
        r = export_plot(escape_time, p_plot_name);
        if (r != 0)
            return r;
        r = export_csv(escape_time, csv_name);
        if (r != 0)
            return r;
    }
    r = export_csv(densities, dimension, p.GetFilePath("dimension_vs_density.csv"));
    if (r != 0)
        return r;
    r = export_csv(densities, plot_dimension, p.GetFilePath("plot_dimension_vs_density.csv"));
    return r;
}

Coord3D<double> ex_dimension_vs_density_thread(double density, ExParam p)
{
    Coord3D<double> output;
    double p_size = (p.density_max - p.density_min) / (double)p.partitions;

    vector<double> escape_time;
    CaHandler ca;
    for (double d = density; d < (density + p_size) && d < 1.0; d += p.dt)
    {
        // Evoluciona el sistema.
        p.args.SetDouble(0, 0.0);
        ca.CreateCa(p.type, p.size, d, p.vmax, p.rand_prob, p.init_vel, p.args, p.random_seed);
        if (ca.Status() != 0)
            return Coord3D<double>(0.0, 0.0, 0.0);

        int iter = 0;
        while (ca.CountCars() != 0)
        {
            ca.Step();
            iter++;
        }
        if (iter != 0)
            escape_time.push_back((double)(ca.GetSize())*d / (double)iter);
    }

    // Obtiene la media y selecciona los puntos mayores e inferiores
    // para generar fractal.
    double et_mean = aux_mean(escape_time);
    vector<int> fractal(escape_time.size(), 0);
    for (unsigned i = 0; i < escape_time.size(); ++i)
    {
        if (escape_time[i] > et_mean)
            fractal[i] = 1;
    }

    // Calcula dimensión fractal.
    double d_mean = density + p_size / 2.0;
    double fractal_dimension = measure_fractal_dimension(fractal, 0, (int)(0.1*fractal.size()), fractal.size(), 1);
    double plot_dimension = measure_plot_fractal_dimension(escape_time, (int)(0.1*fractal.size()), fractal.size(), 1);
    output = Coord3D<double>(d_mean, fractal_dimension, plot_dimension);

    // Exporta los datos.
    string f_path = p.path + "Fractal" + df_separator;
    string f_plot_path = p.path + "Plot" + df_separator;
    string csv_path = p.path + "Csv" + df_separator;
    string p_name = f_path + "discharge_vs_density_fractal_" + to_string(d_mean) + ".bmp";
    string p_plot_name = f_plot_path + "discharge_vs_density_plot_" + to_string(d_mean) + ".bmp";
    string csv_name = csv_path + "discharge_vs_density_" + to_string(d_mean) + ".csv";
    int r;
    r = export_map(fractal, p_name, 30);
    if (r != 0)
        cout << "Error: No se pudo crear mapa." << endl;
    r = export_plot(escape_time, p_plot_name);
    if (r != 0)
        cout << "Error: No se pudo crear grafica." << endl;
    r = export_csv(escape_time, csv_name);
    if (r != 0)
        cout << "Error: No se pudo exportar csv." << endl;

    return output;
}

int ex_dimension_vs_density_parallel(ExParam p)
{
    int r;
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, p.type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        p.type = OPEN_CA;
    }

    // Crea directorios de salida para gráficas.
    aux_create_directory("Fractal");
    aux_create_directory("Plot");
    aux_create_directory("Csv");

    vector<Coord3D<double>> result;
    double p_size = (p.density_max - p.density_min) / (double)p.partitions;
    if (2.0*p.dt >= p_size)
    {
        cout << "Error: dt no es suficientemente pequeño. Se aconseja un valor menor a " << p_size/2.0 << endl;
        return 1;
    }

    result = aux_parallel_function<Coord3D<double>, double, ExParam>(ex_dimension_vs_density_thread, p.density_min, p.density_max, p_size, p);

    vector<Coord<double>> frac_dim, plot_dim;
    for (unsigned i = 0; i < result.size(); ++i)
    {
        // Los valores erróneos son -1.0.
        if (result[i].GetY() >= 0 && result[i].GetZ() >= 0)
        {
            frac_dim.push_back(result[i].SubXY());
            plot_dim.push_back(result[i].SubXZ());
        }
    }

    r = export_csv(frac_dim, p.GetFilePath("dimension_vs_density.csv"));
    if (r != 0)
        return r;

    r = export_csv(plot_dim, p.GetFilePath("plot_dimension_vs_density.csv"));
    return r;
}

int ex_pentropy_vs_density(ExParam p)
{
    CA_TYPE ca_type = p.type;
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, ca_type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        ca_type = OPEN_CA;
    }

    vector<double> density, discharge;
    CaHandler ca;

    for (double d = p.density_min; d <= p.density_max; d += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(d, p.density_min, p.density_max, p.dt);

        // Evoluciona el sistema.
        p.args.SetDouble(0, 0.0);
        ca.CreateCa(ca_type, p.size, d, p.vmax, p.rand_prob, p.init_vel, p.args, p.random_seed);
        if (ca.Status() != 0)
            return 1;

        int iter = 0;
        while (ca.CountCars() != 0)
        {
            ca.Step();
            iter++;
        }
        if (iter != 0)
        {
            density.push_back(d);
            discharge.push_back((double)(ca.GetSize())*d / (double)iter);
        }

    }

    int r = export_data(density, discharge, "discharge_vs_density.csv", p.export_format);
    if (r != 0)
        return r;

    cout << "Midiendo entropia de permutacion." << endl;
    vector<Coord<double>> result = measure_permutation_entropy(density, discharge, p.porder, p.pinterval);
    vector<double> intervalos, entropias;
    for (unsigned i = 0; i < result.size(); i++)
    {
        intervalos.push_back(result[i].GetX());
        entropias.push_back(result[i].GetY());
    }

    return export_data(intervalos, entropias, p.GetFilePath("pentropy_vs_density.csv"), p.export_format);
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
