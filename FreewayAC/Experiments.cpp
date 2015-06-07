#include <iostream>
#include <fstream>
#include <vector>
#include "CellularAutomata.h"
#include "Export.h"
#include "Experiments.h"
using namespace std;

/****************************
*                           *
*          Medición         *
*                           *
****************************/

double measure_fractal_dimension(std::vector<int> frac, int min_div, int max_div, int dt_div)
{
    // Conteo de cajas.
    vector<double> log_epsilon, log_count;
    for (int div = min_div; div <= max_div; div += dt_div)
    {
        double N = 0.0;
        double epsilon = (double)frac.size()/(double)div;
        for(int ex=0; ex<div; ++ex)
        {
            bool found = false;
            for(int w=(int)ex*epsilon; w<(int)(ex+1)*epsilon && !found; ++w)
            {
                if ((unsigned)w < frac.size())
                {
                    if (frac[w] != 0)
                    {
                        N++;
                        found = true;
                        break;
                    }
                }
                else
                    break;
            }
        }
        log_epsilon.push_back(log(1.0/epsilon));
        log_count.push_back(log(N));
    }

    // Ajuste por mínimos cuadrados.
    double size, sum_xy, sum_x, sum_y, sum_x_squared;
    sum_xy = sum_x = sum_y = sum_x_squared = 0;
    size = log_epsilon.size();
    for(int i = 0; i < size; ++i)
    {
        sum_xy += log_epsilon[i]*log_count[i];
        sum_x += log_epsilon[i];
        sum_y += log_count[i];
        sum_x_squared += pow(log_epsilon[i], 2.0);
    }
    double fit = (size*sum_xy - sum_x*sum_y)/(size*sum_x_squared - pow(sum_x,2.0));
    return fit;
}

/****************************
*                           *
*        Experimentos       *
*                           *
****************************/

int ex_traffic_maps(ExParam p)
{
    CellularAutomata* ca = create_ca(p.type, p.size, p.density, p.vmax, p.rand_prob, p.args);
    ca->Evolve(p.iterations);
    int r = ca->DrawHistory(p.path, p.out_file_name);
    delete_ca();
    return r;
}

int ex_flow_maps(ExParam p)
{
    CellularAutomata* ca = create_ca(p.type, p.size, p.density, p.vmax, p.rand_prob, p.args);
    ca->Evolve(p.iterations);
    int r = ca->DrawFlowHistory(p.path, p.out_file_name);
    delete_ca();
    return r;
}

int ex_ocupancy_fixed(ExParam p)
{
    CellularAutomata* ca = create_ca(p.type, p.size, p.density, p.vmax, p.rand_prob, p.args);
    ca->Evolve(p.iterations);

    vector<double> ocupancy;
    ocupancy.assign(ca->GetSize(), 0.0);
    unsigned height = ca->GetHistorySize();
    unsigned width = ca->GetSize();

    for (unsigned i = 0; i < width; ++i)
    {
        int sum = 0;
        for (unsigned j = 1; j < height; ++j)
        {
            if (ca->GetAt(i, j, CA_HISTORY) != -1)
                sum++;
        }
        ocupancy[i] = (double)sum/(double)height;
    }

    // Escribe a CSV.
    if (p.out_file_name.empty())
        p.out_file_name = p.path + "ocupancy.csv";
    else
        p.out_file_name = p.path + p.out_file_name;

    int r = export_csv(ocupancy, p.out_file_name);
    delete_ca();
    return r;
}

int ex_flow_fixed(ExParam p)
{
    CellularAutomata* ca = create_ca(p.type, p.size, p.density, p.vmax, p.rand_prob, p.args);
    ca->Evolve(p.iterations);

    vector<double> flow;
    flow.assign(ca->GetSize(), 0.0);
    unsigned height = ca->GetHistorySize();
    unsigned width = ca->GetSize();

    for (unsigned i = 0; i < width; ++i)
    {
        int sum = 0;
        for (unsigned j = 1; j < height; ++j)
        {
            if ((ca->GetAt(i, j, CA_FLOW_HISTORY) != 0) && (ca->GetAt(i+1, j, CA_FLOW_HISTORY) != 0))
                sum++;
        }
        flow[i] = (double)sum/(double)height;
    }

    // Escribe a CSV.
    if (p.out_file_name.empty())
        p.out_file_name = p.path + "flow.csv";
    else
        p.out_file_name = p.path + p.out_file_name;

    int r = export_csv(flow, p.out_file_name);
    delete_ca();
    return r;
}

int ex_flow_vs_density(ExParam p)
{
    vector<double> density;
    vector<double> flow;
    CellularAutomata* ca;

    for (double d = p.density_min; d <= p.density_max; d += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(d, p.density_min, p.density_max, p.dt);

        // Evoluciona el sistema.
        ca = create_ca(p.type, p.size, d, p.vmax, p.rand_prob, p.args, p.random_seed);
        if (!ca)
            return 1;
        ca->Evolve(p.iterations);

        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(p.size, 0.0);
        unsigned height = ca->GetHistorySize();
        unsigned width = ca->GetSize();

        for (unsigned i = 0; i < width; ++i)
        {
            int sum = 0;
            for (unsigned j = 1; j < height; ++j)
            {
                if ((ca->GetAt(i, j, CA_FLOW_HISTORY) != 0) && (ca->GetAt(i+1, j, CA_FLOW_HISTORY) != 0))
                    sum++;
            }
            tmp_flow[i] = (double)sum/(double)height;
        }

        // Obtiene el promedio de todos los flujos.
        double mean = aux_mean(tmp_flow);
        if (p.per_density) mean /= d;

        // Asigna valores.
        density.push_back(d);
        flow.push_back(mean);
    }

    // Escribe a CSV.
    if (p.out_file_name.empty())
    {
        if (p.per_density)
            p.out_file_name = p.path + "flow_per_density.csv";
        else
            p.out_file_name = p.path + "flow_vs_density.csv";
    }
    else
        p.out_file_name = p.path + p.out_file_name;

    int r = export_csv(density, flow, p.out_file_name);
    delete_ca();
    return r;
}

int ex_multilane_flow_vs_density(ExParam p)
{
    vector<double> density;
    vector<double> flow;
    CellularAutomataML* ca;

    for (double d = p.density_min; d <= p.density_max; d += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(d, p.density_min, p.density_max, p.dt);

        // Evoluciona el sistema.
        ca = create_multilane_ca(p.type, p.size, p.lanes, d, p.vmax, p.rand_prob, p.args, p.random_seed);
        if (!ca)
            return 1;
        ca->Evolve(p.iterations);

        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(p.size, 0.0);
        unsigned height = ca->GetHistorySize();
        unsigned width = ca->GetSize();
        unsigned lane_num = ca->GetLanes();

        for (unsigned i = 0; i < width; ++i)
        {
            int sum = 0;
            for (unsigned j = 0; j<lane_num; ++j)
            {
                for (unsigned k=1; k<height; ++k)
                {
                    if ((ca->GetAt(i, j, k, CA_FLOW_HISTORY) != 0) && (ca->GetAt(i + 1, j, k, CA_FLOW_HISTORY) != 0))
                        sum++;
                }
            }
            tmp_flow[i] = (double)sum / ((double)height*(double)lane_num);
        }

        // Obtiene el promedio de todos los flujos.
        double mean = aux_mean(tmp_flow);
        if (p.per_density) mean /= d;

        // Asigna valores.
        density.push_back(d);
        flow.push_back(mean);
    }

    // Escribe a CSV.
    if (p.out_file_name.empty())
    {
        if (p.per_density)
            p.out_file_name = p.path + "flow_per_density.csv";
        else
            p.out_file_name = p.path + "flow_vs_density.csv";
    }
    else
        p.out_file_name = p.path + p.out_file_name;

    int r = export_csv(density, flow, p.out_file_name);
    delete_ca();
    return r;
}

int ex_flow_vs_vmax(ExParam p)
{
    vector<double> vmax;
    vector<double> flow;
    CellularAutomata* ca;

    for (int v = p.vmax_min; v <= p.vmax_max; v += (int)p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(v, p.vmax_min, p.vmax_max, p.dt);

        // Evoluciona el sistema.
        ca = create_ca(p.type, p.size, p.density, v, p.rand_prob, p.args, p.random_seed);
        if (!ca)
            return 1;
        ca->Evolve(p.iterations);

        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(p.size, 0.0);
        unsigned height = ca->GetHistorySize();
        unsigned width = ca->GetSize();

        for (unsigned i = 0; i < width; ++i)
        {
            int sum = 0;
            for (unsigned j = 1; j < height; ++j)
            {
                if ((ca->GetAt(i, j, CA_FLOW_HISTORY) != 0) && (ca->GetAt(i+1, j, CA_FLOW_HISTORY) != 0))
                    sum++;
            }
            tmp_flow[i] = (double)sum/(double)height;
        }

        // Asigna valores.
        vmax.push_back(v);
        flow.push_back(aux_mean(tmp_flow));
    }

    // Escribe a CSV.
    if (p.out_file_name.empty())
        p.out_file_name = p.path + "flow_vs_vmax.csv";
    else
        p.out_file_name = p.path + p.out_file_name;

    int r = export_csv(vmax, flow, p.out_file_name);
    delete_ca();
    return r;
}

int ex_flow_vs_rand_prob(ExParam p)
{
    vector<double> rand_prob;
    vector<double> flow;
    CellularAutomata* ca;

    for (double r = p.rand_prob_min; r <= p.rand_prob_max; r += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(r, p.rand_prob_min, p.rand_prob_max, p.dt);

        // Evoluciona el sistema.
        ca = create_ca(p.type, p.size, p.density, p.vmax, r, p.args, p.random_seed);
        if (!ca)
            return 1;
        ca->Evolve(p.iterations);

        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(p.size, 0.0);
        unsigned height = ca->GetHistorySize();
        unsigned width = ca->GetSize();

        for (unsigned i = 0; i < width; ++i)
        {
            int sum = 0;
            for (unsigned j = 1; j < height; ++j)
            {
                if ((ca->GetAt(i, j, CA_FLOW_HISTORY) != 0) && (ca->GetAt(i+1, j, CA_FLOW_HISTORY) != 0))
                    sum++;
            }
            tmp_flow[i] = (double)sum/(double)height;
        }

        // Asigna valores.
        rand_prob.push_back(r);
        flow.push_back(aux_mean(tmp_flow));
    }

    // Escribe a CSV.
    if (p.out_file_name.empty())
        p.out_file_name = p.path + "flow_vs_rand_prob.csv";
    else
        p.out_file_name = p.path + p.out_file_name;

    int r = export_csv(rand_prob, flow, p.out_file_name);
    delete_ca();
    return r;
}

int ex_flow_vs_aut_cars(ExParam p)
{
    vector<double> aut_car_density;
    vector<double> flow;
    CellularAutomata* ca;

    for (double s = p.aut_car_density_min; s <= p.aut_car_density_max; s += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(s, p.aut_car_density_min, p.aut_car_density_max, p.dt);

        // Evoluciona el sistema.
        ca = create_ca(AUTONOMOUS_CA, p.size, p.density, p.vmax, p.rand_prob, Args({ s }), p.random_seed);
        if (!ca)
            return 1;
        ca->Evolve(p.iterations);

        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(p.size, 0.0);
        unsigned height = ca->GetHistorySize();
        unsigned width = ca->GetSize();

        for (unsigned i = 0; i < width; ++i)
        {
            int sum = 0;
            for (unsigned j = 1; j < height; ++j)
            {
                if ((ca->GetAt(i, j, CA_FLOW_HISTORY) != 0) && (ca->GetAt(i+1, j, CA_FLOW_HISTORY) != 0))
                    sum++;
            }
            tmp_flow[i] = (double)sum/(double)height;
        }

        // Asigna valores.
        aut_car_density.push_back(s);
        flow.push_back(aux_mean(tmp_flow));
    }

    // Escribe a CSV.
    if (p.out_file_name.empty())
        p.out_file_name = p.path + "flow_vs_aut_cars.csv";
    else
        p.out_file_name = p.path + p.out_file_name;

    int r = export_csv(aut_car_density, flow, p.out_file_name);
    delete_ca();
    return r;
}

int ex_flow_vs_new_car_prob(ExParam p)
{
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, p.type))
    {
        cout << "AC no valido para experimento seleccionado." << endl;
        return 1;
    }

    vector<double> new_car_density;
    vector<double> flow;
    CellularAutomata* ca;

    for (double s = p.new_car_prob_min; s <= p.new_car_prob_max; s += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(s, p.new_car_prob_min, p.new_car_prob_max, p.dt);

        // Evoluciona el sistema.
        p.args.SetDouble(0, s);
        ca = create_ca(p.type, p.size, p.density, p.vmax, p.rand_prob, p.args, p.random_seed);
        if (!ca)
            return 1;
        ca->Evolve(p.iterations);

        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(p.size, 0.0);
        unsigned height = ca->GetHistorySize();
        unsigned width = ca->GetSize();

        for (unsigned i = 0; i < width; ++i)
        {
            int sum = 0;
            for (unsigned j = 1; j < height; ++j)
            {
                if ((ca->GetAt(i, j, CA_FLOW_HISTORY) != 0) && (ca->GetAt(i+1, j, CA_FLOW_HISTORY) != 0))
                    sum++;
            }
            tmp_flow[i] = (double)sum/(double)height;
        }

        // Obtiene el promedio de todos los flujos.
        double mean = aux_mean(tmp_flow);
        if (p.per_prob)
            mean /= s;

        // Asigna valores.
        new_car_density.push_back(s);
        flow.push_back(mean);
    }

    // Escribe a CSV.
    if (p.out_file_name.empty())
    {
        if (p.per_prob)
            p.out_file_name = p.path + "flow_per_new_car_prob.csv";
        else
            p.out_file_name = p.path + "flow_vs_new_car_prob.csv";
    }
    else
        p.out_file_name = p.path + p.out_file_name;

    int r = export_csv(new_car_density, flow, p.out_file_name);
    delete_ca();
    return r;
}

int ex_flow_vs_stop_density(ExParam p)
{
    vector<double> stop_density;
    vector<double> flow;
    CellularAutomata* ca;

    for (double d = p.stop_density_min; d <= p.stop_density_max; d += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(d, p.stop_density_min, p.stop_density_max, p.dt);

        // Evoluciona el sistema.
        ca = create_ca(STOP_CA, p.size, p.density, p.vmax, p.rand_prob, Args({ d }), p.random_seed);
        if (!ca)
            return 1;
        ca->Evolve(p.iterations);

        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(p.size, 0.0);
        unsigned height = ca->GetHistorySize();
        unsigned width = ca->GetSize();

        for (unsigned i = 0; i < width; ++i)
        {
            int sum = 0;
            for (unsigned j = 1; j < height; ++j)
            {
                if ((ca->GetAt(i, j, CA_FLOW_HISTORY) != 0) && (ca->GetAt(i+1, j, CA_FLOW_HISTORY) != 0))
                    sum++;
            }
            tmp_flow[i] = (double)sum/(double)height;
        }

        // Asigna valores.
        stop_density.push_back(d);
        flow.push_back(aux_mean(tmp_flow));
    }

    // Escribe a CSV.
    if (p.out_file_name.empty())
        p.out_file_name = p.path + "flow_vs_stop_density.csv";
    else
        p.out_file_name = p.path + p.out_file_name;

    int r = export_csv(stop_density, flow, p.out_file_name);
    delete_ca();
    return r;
}

int ex_flow_vs_semaphore_density(ExParam p)
{
    vector<double> semaphore_density;
    vector<double> flow;
    CellularAutomata* ca;

    for (double d = p.semaphore_density_min; d <= p.semaphore_density_max; d += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(d, p.semaphore_density_min, p.semaphore_density_max, p.dt);

        // Evoluciona el sistema.
        ca = create_ca(SEMAPHORE_CA, p.size, p.density, p.vmax, p.rand_prob, Args({ d }, {}, { p.random_semaphores }), p.random_seed);
        if (!ca)
            return 1;
        ca->Evolve(p.iterations);

        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(p.size, 0.0);
        unsigned height = ca->GetHistorySize();
        unsigned width = ca->GetSize();

        for (unsigned i = 0; i < width; ++i)
        {
            int sum = 0;
            for (unsigned j = 1; j < height; ++j)
            {
                if ((ca->GetAt(i, j, CA_FLOW_HISTORY) != 0) && (ca->GetAt(i+1, j, CA_FLOW_HISTORY) != 0))
                    sum++;
            }
            tmp_flow[i] = (double)sum/(double)height;
        }

        // Asigna valores.
        semaphore_density.push_back(d);
        flow.push_back(aux_mean(tmp_flow));
    }

    // Escribe a CSV.
    if (p.out_file_name.empty())
        p.out_file_name = p.path + "flow_vs_semaphore_density.csv";
    else
        p.out_file_name = p.path + p.out_file_name;

    int r = export_csv(semaphore_density, flow, p.out_file_name);
    delete_ca();
    return r;
}


int ex_escape_time_vs_density(ExParam p)
{
    CA_TYPE ca_type = p.type;
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, ca_type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        ca_type = OPEN_CA;
    }

    vector<double> densities;
    vector<double> escape_time;
    CellularAutomata* ca;

    for (double d = p.density_min; d <= p.density_max; d += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(d, p.density_min, p.density_max, p.dt);

        // Evoluciona el sistema.
        p.args.SetDouble(0, 0.0);
        ca = create_ca(ca_type, p.size, d, p.vmax, p.rand_prob, p.args, p.random_seed);
        if (!ca)
            return 1;

        int iter = 0;
        while (ca->CountCars() != 0)
        {
            ca->Step();
            iter++;

            if (ca->IsFluxHalted())
            {
                cout << "Error: Flujo detenido." << endl;
                return 1;
            }
        }
        densities.push_back(d);
        escape_time.push_back(iter);

    }

    int r = export_csv(densities, escape_time, "escape_time_vs_density.csv");
    delete_ca();
    return r;
}

int ex_escape_time_vs_rand_prob(ExParam p)
{
    CA_TYPE ca_type = p.type;
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, ca_type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        ca_type = OPEN_CA;
    }

    vector<double> rand_p;
    vector<double> escape_time;
    CellularAutomata* ca;

    for (double ra = p.rand_prob_min; ra <= p.rand_prob_max; ra += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(ra, p.rand_prob_min, p.rand_prob_max, p.dt);

        // Evoluciona el sistema.
        p.args.SetDouble(0, 0.0);
        ca = create_ca(ca_type, p.size, p.density, p.vmax, ra, p.args, p.random_seed);
        if (!ca)
            return 1;

        int iter = 0;
        while (ca->CountCars() != 0 && !ca->IsFluxHalted())
        {
            ca->Step();
            iter++;
        }
        rand_p.push_back(ra);
        escape_time.push_back(iter);

    }

    int r = export_csv(rand_p, escape_time, "escape_time_vs_rand_prob.csv");
    delete_ca();
    return r;
}

int ex_escape_time_vs_vmax(ExParam p)
{
    CA_TYPE ca_type = p.type;
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, ca_type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        ca_type = OPEN_CA;
    }

    vector<double> vel;
    vector<double> escape_time;
    CellularAutomata* ca;

    for (int v = p.vmax_min; v <= p.vmax_max; v += (int)p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(v, p.vmax_min, p.vmax_max, p.dt);

        // Evoluciona el sistema.
        p.args.SetDouble(0, 0.0);
        ca = create_ca(ca_type, p.size, p.density, v, p.rand_prob, p.args, p.random_seed);
        if (!ca)
            return 1;

        int iter = 0;
        while (ca->CountCars() != 0 && !ca->IsFluxHalted())
        {
            ca->Step();
            iter++;
        }
        vel.push_back(v);
        escape_time.push_back(iter);

    }

    int r = export_csv(vel, escape_time, "escape_time_vs_vmax.csv");
    delete_ca();
    return r;
}

int ex_discharge_vs_density(ExParam p)
{
    CA_TYPE ca_type = p.type;
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, ca_type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        ca_type = OPEN_CA;
    }

    vector<double> densities;
    vector<double> escape_time;
    CellularAutomata* ca;

    for (double d = p.density_min; d <= p.density_max; d += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(d, p.density_min, p.density_max, p.dt);

        // Evoluciona el sistema.
        p.args.SetDouble(0, 0.0);
        ca = create_ca(ca_type, p.size, d, p.vmax, p.rand_prob, p.args, p.random_seed);
        if (!ca)
            return 1;

        int iter = 0;
        while (ca->CountCars() != 0)
        {
            ca->Step();
            iter++;

            if (ca->IsFluxHalted())
            {
                cout << "Error: Flujo detenido." << endl;
                return 1;
            }
        }
        if (iter != 0)
        {
            densities.push_back(d);
            escape_time.push_back((double)(ca->GetSize())*d/(double)iter);
        }

    }

    int r = export_csv(densities, escape_time, "discharge_vs_density.csv");
    delete_ca();
    return r;
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
    CellularAutomata* ca;

    for (double d = p.density_min; d <= p.density_max; d += p.dt)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(d, p.density_min, p.density_max, p.dt);

        // Evoluciona el sistema.
        p.args.SetDouble(0, 0.0);
        ca = create_ca(ca_type, p.size, d, p.vmax, p.rand_prob, p.args, p.random_seed);
        if (!ca)
            return 1;

        int iter = 0;
        while (ca->CountCars() != 0)
        {
            ca->Step();
            iter++;

            if (ca->IsFluxHalted())
            {
                cout << "Error: Flujo detenido." << endl;
                return 1;
            }
        }
        if (iter != 0)
            escape_time.push_back((double)(ca->GetSize())*d/(double)iter);
    }

    cout << "Calculando dimensión fractal." << endl;
    double et_mean = aux_mean(escape_time);
    vector<int> fractal(escape_time.size(), 0);
    for (unsigned i = 0; i < escape_time.size(); ++i)
    {
        if (escape_time[i] > et_mean)
            fractal[i] = 1;
    }
	cout << "La dimension fractal es: " << measure_fractal_dimension(fractal, (int)0.1*fractal.size(), fractal.size(), 1);
	cout << "." << endl;

    int r = export_plot(fractal, "discharge_vs_density_fractal.bmp", 30, false, BINARY_COLORS);
    delete_ca();
    return r;
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

    vector<double> densities, dimension;
    double p_size = (p.density_max-p.density_min)/(double)p.partitions;
    CellularAutomata* ca;

    int part = 0;
    for (double d_left = p.density_min; part < p.partitions; d_left += p_size, part++)
    {
        // Reporta progreso.
        if (p.show_progress)
            aux_progress_bar(part, 0, p.partitions-1, 1);

        vector<double> escape_time;
        for (double d = d_left; d < (d_left+p_size); d += p.dt)
        {
            // Evoluciona el sistema.
            p.args.SetDouble(0, 0.0);
            ca = create_ca(ca_type, p.size, d, p.vmax, p.rand_prob, p.args, p.random_seed);
            if (!ca)
                return 1;

            int iter = 0;
            while (ca->CountCars() != 0)
            {
                ca->Step();
                iter++;

                if (ca->IsFluxHalted())
                {
                    cout << "Error: Flujo detenido." << endl;
                    return 1;
                }
            }
            if (iter != 0)
                escape_time.push_back((double)(ca->GetSize())*d/(double)iter);
        }

        double et_mean = aux_mean(escape_time);
        vector<int> fractal(escape_time.size(), 0);
        for (unsigned i = 0; i < escape_time.size(); ++i)
        {
            if (escape_time[i] > et_mean)
                fractal[i] = 1;
        }

        double d_mean = d_left + p_size/2.0;
        densities.push_back(d_mean);
        dimension.push_back(measure_fractal_dimension(fractal, (int)0.1*fractal.size(), fractal.size(), 1));

        if (p.path.empty())
        {
            aux_create_directory("Fractal");
            p.path = "Fractal" + f_separator;
        }

        string p_name = p.path + "discharge_vs_density_fractal_" + to_string(d_mean) + ".bmp";
        r = export_plot(fractal, p_name, 30, false, BINARY_COLORS);
        if (r != 0)
            return 1;
    }

    r = export_csv(densities, dimension, "dimension_vs_density.csv");
    delete_ca();
    return r;
}


/**
* @brief Realiza tests para verificar la corrección de los algoritmos.
*/
void ex_perform_test()
{
    // Circular.
    vector<int> init({1, -1, -1, 1, -1, 1, 1, -1, -1, 1, -1, -1, -1, 1, -1, -1, 1, -1, -1, -1});
    vector<int> end_circ({-1, 2, -1, 1, 0, -1, -1, 1, -1, 1, -1,-1, -1, -1, 2, -1, -1, -1, 2, -1});
    vector<bool> rand_val_circ({false, false, false, false, true, false, true,
                                    true, false, false, true, false, false, false,
                                    false, true, false, false, true, false, false});

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
    vector<bool> rand_val_open_vec({false, false, false, false, true, false, true, false,
                                    true, false, false, true, false, false, false, true,
                                    false, true, false, false, true, false, false, false});

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
