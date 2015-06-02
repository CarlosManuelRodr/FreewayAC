#include <iostream>
#include <fstream>
#include <vector>
#include "CellularAutomata.h"
#include "Export.h"
using namespace std;

/****************************
*                           *
*        Experimentos       *
*                           *
****************************/

int ex_ocupancy_fixed(CellularAutomata *ca, string path, string out_file_name)
{
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
    if (out_file_name.empty())
        out_file_name = path + "ocupancy.csv";
    else
        out_file_name = path + out_file_name;

    int r = export_csv(ocupancy, out_file_name);
    return r;
}

int ex_flow_fixed(CellularAutomata *ca, string path = "", string out_file_name = "")
{
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
    if (out_file_name.empty())
        out_file_name = path + "flow.csv";
    else
        out_file_name = path + out_file_name;

    int r = export_csv(flow, out_file_name);
    return r;
}

int ex_flow_vs_density(const CA_TYPE &type, const unsigned &size, const unsigned &iterations, const int &vmax,
                       const double &density_min, const double &density_max, const double &dt, const double &rand_prob,
                       Args args, const bool &per_density, const int &random_seed, const bool &show_progress, string path,
                       string out_file_name)
{
    vector<double> density;
    vector<double> flow;
    CellularAutomata* ca;

    for (double d = density_min; d <= density_max; d += dt)
    {
        // Reporta progreso.
    	if (show_progress)
    		aux_progress_bar(d, density_min, density_max, dt);

        // Evoluciona el sistema.
        ca = create_ca(type, size, d, vmax, rand_prob, args, random_seed);
        if (!ca)
        	return 1;
        ca->Evolve(iterations);

        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(size, 0.0);
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
        if (per_density) mean /= d;

        // Asigna valores.
        density.push_back(d);
        flow.push_back(mean);
    }

    // Escribe a CSV.
    if (out_file_name.empty())
    {
        if (per_density)
            out_file_name = path + "flow_per_density.csv";
        else
            out_file_name = path + "flow_vs_density.csv";
    }
    else
        out_file_name = path + out_file_name;

    int r = export_csv(density, flow, out_file_name);
    delete_ca();
    return r;
}

int ex_multilane_flow_vs_density(const CA_TYPE &type, const unsigned &size, const unsigned &lanes, const unsigned &iterations,
                                 const int &vmax, const double &density_min, const double &density_max, const double &dt,
                                 const double &rand_prob, Args args, const bool &per_density, const int &random_seed,
                                 const bool &show_progress, string path, string out_file_name)
{
    vector<double> density;
    vector<double> flow;
    CellularAutomataML* ca;

    for (double d = density_min; d <= density_max; d += dt)
    {
        // Reporta progreso.
    	if (show_progress)
    		aux_progress_bar(d, density_min, density_max, dt);

        // Evoluciona el sistema.
        ca = create_multilane_ca(type, size, lanes, d, vmax, rand_prob, args, random_seed);
        if (!ca)
			return 1;
        ca->Evolve(iterations);

        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(size, 0.0);
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
        if (per_density) mean /= d;

        // Asigna valores.
        density.push_back(d);
        flow.push_back(mean);
    }

    // Escribe a CSV.
    if (out_file_name.empty())
    {
        if (per_density)
            out_file_name = path + "flow_per_density.csv";
        else
            out_file_name = path + "flow_vs_density.csv";
    }
    else
        out_file_name = path + out_file_name;

    int r = export_csv(density, flow, out_file_name);
    delete_ca();
    return r;
}

int ex_flow_vs_vmax(const CA_TYPE &type, const unsigned &size, const unsigned &iterations, const int &vmax_min,
                    const int &vmax_max, const int &dt, const double &density, const double &rand_prob,
                    Args args, const int &random_seed, const bool &show_progress, string path,
                    string out_file_name)
{
    vector<double> vmax;
    vector<double> flow;
    CellularAutomata* ca;

    for (int v = vmax_min; v <= vmax_max; v += dt)
    {
        // Reporta progreso.
    	if (show_progress)
    		aux_progress_bar(v, vmax_min, vmax_max, dt);

        // Evoluciona el sistema.
        ca = create_ca(type, size, density, v, rand_prob, args, random_seed);
        if (!ca)
			return 1;
        ca->Evolve(iterations);

        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(size, 0.0);
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
    if (out_file_name.empty())
        out_file_name = path + "flow_vs_vmax.csv";
    else
        out_file_name = path + out_file_name;

    int r = export_csv(vmax, flow, out_file_name);
    delete_ca();
    return r;
}

int ex_flow_vs_rand_prob(const CA_TYPE &type, const unsigned &size, const unsigned &iterations, const int &vmax,
                         const double &density, const double &rand_prob_min, const double &rand_prob_max,
                         const double &dt, Args args, const int &random_seed, const bool &show_progress, string path,
                         string out_file_name)
{
    vector<double> rand_prob;
    vector<double> flow;
    CellularAutomata* ca;

    for (double r = rand_prob_min; r <= rand_prob_max; r += dt)
    {
        // Reporta progreso.
    	if (show_progress)
    		aux_progress_bar(r, rand_prob_min, rand_prob_max, dt);

        // Evoluciona el sistema.
        ca = create_ca(type, size, density, vmax, r, args, random_seed);
        if (!ca)
			return 1;
        ca->Evolve(iterations);

        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(size, 0.0);
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
    if (out_file_name.empty())
        out_file_name = path + "flow_vs_rand_prob.csv";
    else
        out_file_name = path + out_file_name;

    int r = export_csv(rand_prob, flow, out_file_name);
    delete_ca();
    return r;
}

int ex_flow_vs_aut_cars(const unsigned &size, const unsigned &iterations, const int &vmax, const double &density,
                        const double &rand_prob, const double &aut_car_density_min, const double &aut_car_density_max,
                        const double &dt, const int &random_seed, const bool &show_progress, string path,
                        string out_file_name)
{
    vector<double> aut_car_density;
    vector<double> flow;
    CellularAutomata* ca;

    for (double s = aut_car_density_min; s <= aut_car_density_max; s += dt)
    {
        // Reporta progreso.
    	if (show_progress)
    		aux_progress_bar(s, aut_car_density_min, aut_car_density_max, dt);

        // Evoluciona el sistema.
        ca = create_ca(AUTONOMOUS_CA, size, density, vmax, rand_prob, Args({ s }), random_seed);
        if (!ca)
			return 1;
        ca->Evolve(iterations);

        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(size, 0.0);
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
    if (out_file_name.empty())
        out_file_name = path + "flow_vs_aut_cars.csv";
    else
        out_file_name = path + out_file_name;

    int r = export_csv(aut_car_density, flow, out_file_name);
    delete_ca();
    return r;
}

int ex_flow_vs_new_car_prob(const CA_TYPE &type, const unsigned &size, const unsigned &iterations, const int &vmax,
		                    const double &density, const double &rand_prob, const double &new_car_prob_min,
		                    const double &new_car_prob_max, const double &dt, Args args, const bool &per_prob,
		                    const int &random_seed, const bool &show_progress, string path, string out_file_name)
{
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, type))
    {
        cout << "AC no valido para experimento seleccionado." << endl;
        return 1;
    }

    vector<double> new_car_density;
    vector<double> flow;
    CellularAutomata* ca;

    for (double s = new_car_prob_min; s <= new_car_prob_max; s += dt)
    {
        // Reporta progreso.
    	if (show_progress)
    		aux_progress_bar(s, new_car_prob_min, new_car_prob_max, dt);

        // Evoluciona el sistema.
        args.SetDouble(0, s);
        ca = create_ca(type, size, density, vmax, rand_prob, args, random_seed);
        if (!ca)
			return 1;
        ca->Evolve(iterations);

        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(size, 0.0);
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
        if (per_prob)
            mean /= s;

        // Asigna valores.
        new_car_density.push_back(s);
        flow.push_back(mean);
    }

    // Escribe a CSV.
    if (out_file_name.empty())
    {
        if (per_prob)
            out_file_name = path + "flow_per_new_car_prob.csv";
        else
            out_file_name = path + "flow_vs_new_car_prob.csv";
    }
    else
        out_file_name = path + out_file_name;

    int r = export_csv(new_car_density, flow, out_file_name);
    delete_ca();
    return r;
}

int ex_flow_vs_stop_density(const unsigned &size, const unsigned &iterations, const int &vmax,
                            const double &density, const double &rand_prob, const double &stop_density_min,
                            const double &stop_density_max, const double &dt, const int &random_seed,
                            const bool &show_progress, string path, string out_file_name)
{
    vector<double> stop_density;
    vector<double> flow;
    CellularAutomata* ca;

    for (double d = stop_density_min; d <= stop_density_max; d += dt)
    {
        // Reporta progreso.
    	if (show_progress)
    		aux_progress_bar(d, stop_density_min, stop_density_max, dt);

        // Evoluciona el sistema.
        ca = create_ca(STOP_CA, size, density, vmax, rand_prob, Args({ d }), random_seed);
        if (!ca)
			return 1;
        ca->Evolve(iterations);

        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(size, 0.0);
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
    if (out_file_name.empty())
        out_file_name = path + "flow_vs_stop_density.csv";
    else
        out_file_name = path + out_file_name;

    int r = export_csv(stop_density, flow, out_file_name);
    delete_ca();
    return r;
}

int ex_flow_vs_semaphore_density(const unsigned &size, const unsigned &iterations, const int &vmax,
                                 const double &density, const double &rand_prob, const double &semaphore_density_min,
                                 const double &semaphore_density_max, const double &dt, const bool &random_semaphores,
                                 const int &random_seed, const bool &show_progress, string path, string out_file_name)
{
    vector<double> semaphore_density;
    vector<double> flow;
    CellularAutomata* ca;

    for (double d = semaphore_density_min; d <= semaphore_density_max; d += dt)
    {
        // Reporta progreso.
    	if (show_progress)
    		aux_progress_bar(d, semaphore_density_min, semaphore_density_max, dt);

        // Evoluciona el sistema.
        ca = create_ca(SEMAPHORE_CA, size, density, vmax, rand_prob, Args({ d }, {}, { random_semaphores }), random_seed);
        if (!ca)
			return 1;
        ca->Evolve(iterations);

        // Obtiene flujo en cada posición.
        vector<double> tmp_flow;
        tmp_flow.assign(size, 0.0);
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
    if (out_file_name.empty())
        out_file_name = path + "flow_vs_semaphore_density.csv";
    else
        out_file_name = path + out_file_name;

    int r = export_csv(semaphore_density, flow, out_file_name);
    delete_ca();
    return r;
}


int ex_escape_time_vs_density(const CA_TYPE &type, const unsigned &size, const int &vmax,
		                      const double &density_min, const double &density_max, const double &dt,
		                      const double &rand_prob, Args args, const int &random_seed,
		                      const bool &show_progress, string path, string out_file_name)
{
	CA_TYPE ca_type = type;
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, ca_type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        ca_type = OPEN_CA;
    }

    vector<double> densities;
    vector<double> escape_time;
    CellularAutomata* ca;

    for (double d = density_min; d <= density_max; d += dt)
    {
        // Reporta progreso.
    	if (show_progress)
    		aux_progress_bar(d, density_min, density_max, dt);

        // Evoluciona el sistema.
        args.SetDouble(0, 0.0);
        ca = create_ca(ca_type, size, d, vmax, rand_prob, args, random_seed);
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

int ex_escape_time_vs_rand_prob(const CA_TYPE &type, const unsigned &size, const double &density, const int &vmax,
		                        const double &rand_prob_min, const double &rand_prob_max, const double &dt,
		                        Args args, const int &random_seed, const bool &show_progress, string path,
		                        string out_file_name)
{
	CA_TYPE ca_type = type;
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, ca_type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        ca_type = OPEN_CA;
    }

    vector<double> rand_p;
    vector<double> escape_time;
    CellularAutomata* ca;

    for (double ra = rand_prob_min; ra <= rand_prob_max; ra += dt)
    {
        // Reporta progreso.
        if (show_progress)
        	aux_progress_bar(ra, rand_prob_min, rand_prob_max, dt);

        // Evoluciona el sistema.
        args.SetDouble(0, 0.0);
        ca = create_ca(ca_type, size, density, vmax, ra, args, random_seed);
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

int ex_escape_time_vs_vmax(const CA_TYPE &type, const unsigned &size, const double &density, const int &vmax_min,
                           const int &vmax_max, const int &dt, const double &rand_prob,
		                   Args args, const int &random_seed, const bool &show_progress, string path,
		                   string out_file_name)
{
	CA_TYPE ca_type = type;
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, ca_type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        ca_type = OPEN_CA;
    }

    vector<double> vel;
    vector<double> escape_time;
    CellularAutomata* ca;

    for (int v = vmax_min; v <= vmax_max; v += dt)
    {
        // Reporta progreso.
        if (show_progress)
        	aux_progress_bar(v, vmax_min, vmax_max, dt);

        // Evoluciona el sistema.
        args.SetDouble(0, 0.0);
        ca = create_ca(ca_type, size, density, v, rand_prob, args, random_seed);
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

int ex_discharge_vs_density(const CA_TYPE &type, const unsigned &size, const int &vmax,
		                    const double &density_min, const double &density_max, const double &dt,
		                    const double &rand_prob, Args args, const int &random_seed,
		                    const bool &show_progress, string path, string out_file_name)
{
	CA_TYPE ca_type = type;
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, ca_type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        ca_type = OPEN_CA;
    }

    vector<double> densities;
    vector<double> escape_time;
    CellularAutomata* ca;

    for (double d = density_min; d <= density_max; d += dt)
    {
        // Reporta progreso.
    	if (show_progress)
    		aux_progress_bar(d, density_min, density_max, dt);

        // Evoluciona el sistema.
        args.SetDouble(0, 0.0);
        ca = create_ca(ca_type, size, d, vmax, rand_prob, args, random_seed);
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

int ex_discharge_vs_density_fratal(const CA_TYPE &type, const unsigned &size, const int &vmax,
		                           const double &density_min, const double &density_max, const double &dt,
		                           const double &rand_prob, Args args, const int &random_seed,
		                           const bool &show_progress, string path, string out_file_name)
{
	CA_TYPE ca_type = type;
    if (!aux_is_in<CA_TYPE>({ OPEN_CA, SIMPLE_JUNCTION_CA }, ca_type))
    {
        cout << "AC no valido para experimento seleccionado. Cambiando a ca_open." << endl;
        ca_type = OPEN_CA;
    }

    vector<double> escape_time;
    CellularAutomata* ca;

    for (double d = density_min; d <= density_max; d += dt)
    {
        // Reporta progreso.
    	if (show_progress)
    		aux_progress_bar(d, density_min, density_max, dt);

        // Evoluciona el sistema.
        args.SetDouble(0, 0.0);
        ca = create_ca(ca_type, size, d, vmax, rand_prob, args, random_seed);
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

    int r = export_plot(fractal, "discharge_vs_density_fractal.bmp", 30, false, BINARY_COLORS);
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
