#include <sstream>
#include "CellularAutomata.h"
#include "optionparser.h"
using namespace std;


/****************************
*                           *
*   Funciones de medicion   *
*                           *
****************************/

void single_measure_ocupancy(CellularAutomata *ca)
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
    ofstream file("ocupancy.csv", ofstream::out);
    for (unsigned i = 0; i < ocupancy.size(); ++i)
    {
        if (i - ocupancy.size() != 0)
            file << ocupancy[i] << ", ";
        else
            file << ocupancy[i];
    }

    file.close();
}
void single_measure_flow(CellularAutomata *ca)
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
    ofstream file("flow.csv", ofstream::out);
    for (unsigned i = 0; i < flow.size(); ++i)
    {
        if (i - flow.size() != 0)
            file << flow[i] << ", ";
        else
            file << flow[i];
    }

    file.close();
}

void multiple_flow_vs_density(const CA_TYPE &type, const unsigned &size, const unsigned &iterations, const int &vmax, 
                              const double &density_min, const double &density_max, const double &dt, const double &rand_prob, 
                              const double &extra, const bool &per_density = false)
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
        ca = create_ca(type, size, d, vmax, rand_prob);
        for (unsigned i=0; i<iterations; ++i)
            ca->Step();


        // Obtiene flujo en cada posicion.
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
    string filename;
    if (per_density)
        filename = "flow_per_density.csv";
    else
        filename = "flow_vs_density.csv";

    ofstream file(filename.c_str(), ofstream::out);
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
void multiple_flow_vs_vmax(const CA_TYPE &type, const unsigned &size, const unsigned &iterations, const int &vmax_min, const int &vmax_max, 
                           const int &dt, const double &density, const double &rand_prob, const double &extra)
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
        ca = create_ca(type, size, density, v, rand_prob);
        for (unsigned i=0; i<iterations; ++i)
            ca->Step();


        // Obtiene flujo en cada posicion.
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
    ofstream file("flow_vs_vmax.csv", ofstream::out);
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
void multiple_flow_vs_rand_prob(const CA_TYPE &type, const unsigned &size, const unsigned &iterations, const int &vmax, const double &density, 
                                const double &rand_prob_min, const double &rand_prob_max, const double &dt, const double &extra)
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
        ca = create_ca(type, size, density, vmax, r);
        for (unsigned i=0; i<iterations; ++i)
            ca->Step();


        // Obtiene flujo en cada posicion.
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
    ofstream file("flow_vs_rand_prob.csv", ofstream::out);
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
void multiple_flow_vs_smart_cars(const unsigned &size, const unsigned &iterations, const int &vmax, const double &density, 
                                 const double &rand_prob, const double &smart_car_density_min, const double &smart_car_density_max, 
                                 const double &dt)
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
        ca = create_ca(SMART_CA, size, density, vmax, rand_prob, s);
        for (unsigned i=0; i<iterations; ++i)
            ca->Step();


        // Obtiene flujo en cada posicion.
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
    ofstream file("flow_vs_smart_car_density.csv", ofstream::out);
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
void multiple_flow_vs_new_car_prob(const unsigned &size, const unsigned &iterations, const int &vmax, const double &density, 
                                   const double &rand_prob, const double &new_car_density_min, const double &new_car_density_max, 
                                   const double &dt)
{
    vector<double> new_car_density;
    vector<double> flow;
    CellularAutomata* ca;

    for (double s=new_car_density_min; s<=new_car_density_max; s+=dt)
    {
        // Reporta progreso.
        if (s + dt > new_car_density_max)
            aux_progress_bar(1.0);
        else
            aux_progress_bar(s/new_car_density_max);

        // Evoluciona el sistema.
        ca = create_ca(OPEN_CA, size, density, vmax, rand_prob, s);
        for (unsigned i=0; i<iterations; ++i)
            ca->Step();


        // Obtiene flujo en cada posicion.
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
    ofstream file("flow_vs_smart_new_density.csv", ofstream::out);
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


/****************************
*                           *
*   Parser de parametros    *
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
                    FLOW_VS_DENSITY, FLOW_PER_DENSITY, FLOW_VS_VMAX, FLOW_VS_RAND_PROB, FLOW_VS_SMART_CARS, FLOW_VS_NEW_CAR,
                    CA_CIRCULAR, CA_OPEN, CA_SMART, CA_STOP, NEW_CAR_PROB, SMART_DENSITY, DT, DMIN, DMAX, VMAX_MIN, VMAX_MAX, 
                    RAND_PROB_MIN, SMART_MIN, SMART_MAX, STOP_DENSITY, NEW_CAR_MIN, NEW_CAR_MAX, RAND_PROB_MAX, HELP };

const option::Descriptor usage[] =
{
    {UNKNOWN, 0, "", "",Arg::None, "INSTRUCCIONES: FreewayAC [opciones]\n"},
    {SIZE,  0,"s", "size", Arg::Required, "  -s <arg>, \t--size=<arg>  \tTamagno del AC." },
    {ITERATIONS,  0,"i", "iter", Arg::Required, "  -i <arg>, \t--iter=<arg>  \tIteraciones del AC." },
    {VMAX,  0,"v", "vmax", Arg::Required, "  -v <arg>, \t--vmax=<arg>  \tVelocidad maxima de auto." },
    {DENSITY,  0,"d", "density", Arg::Required, "  -d <arg>, \t--density=<arg>  \tDensidad de autos." },
    {RAND_PROB,  0,"r", "rand_prob", Arg::Required, "  -r <arg>, \t--rand_prob=<arg>  \tProbabilidad de descenso de velocidad." },
    {PLOT_TRAFFIC,  0,"p","plot_traffic", Arg::None, "  -p , \t--plot_traffic  \tCrea mapa de autos vs tiempo y flujo vs tiempo." },
    {MEASURE_OCUPANCY,  0,"o","measure_ocupancy", Arg::None, "  -o , \t--measure_ocupancy  \tMide la densidad de ocupacion promedio de cada casilla." },
    {MEASURE_FLOW,  0,"f","measure_flow", Arg::None, "  -f , \t--measure_flow  \tMide el flujo promedio de cada casilla." },
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
    {FLOW_VS_NEW_CAR,  0,"","flow_vs_new_car", Arg::None, 
     "  \t--flow_vs_new_car  \tFlujo respecto a probabilidad de nuevo auto en ac abierto en un rango especificado por new_car_min, new_car_max y dt." },
    {CA_CIRCULAR,  0,"","ca_circular", Arg::None, "  \t--ca_circular  \tAutomata celular circular." },
    {CA_OPEN,  0,"","ca_open", Arg::None, "  \t--ca_open  \tAutomata celular con frontera abierta." },
    {CA_SMART,  0,"","ca_smart", Arg::None, "  \t--ca_smart  \tAutomata celular con autos inteligentes." },
	{CA_STOP,  0,"","ca_stop", Arg::None, "  \t--ca_stop  \tAutomata celular con tope. La posicion del tope se especifica por stop_pos." },
    {NEW_CAR_PROB,  0,"","new_car_prob", Arg::Required, "  \t--new_car_prob  \tProbabilidad de que se aparezca nuevo auto en frontera abierta." },
    {SMART_DENSITY,  0,"","smart_density", Arg::Required, "  \t--smart_density  \tDensidad de autos inteligentes." },
    {DT,  0,"", "dt", Arg::Required, "  \t--dt=<arg>  \tTamagno del intervalo en medicion con rango." },
    {DMIN,  0,"", "dmin", Arg::Required, "  \t--dmin=<arg>  \tDensidad minima en medicion con rango." },
    {DMAX,  0,"", "dmax", Arg::Required, "  \t--dmax=<arg>  \tDensidad maxima en medicion con rango." },
    {VMAX_MIN,  0,"", "vmax_min", Arg::Required, "  \t--vmax_min=<arg>  \tVmax minima en medicion con rango." },
    {VMAX_MAX,  0,"", "vmax_max", Arg::Required, "  \t--vmax_max=<arg>  \tVmax maxima en medicion con rango." },
    {RAND_PROB_MIN,  0,"", "rand_min", Arg::Required, "  \t--rand_min=<arg>  \tProbabilidad de frenado minima en medicion con rango." },
    {RAND_PROB_MAX,  0,"", "rand_max", Arg::Required, "  \t--rand_max=<arg>  \tProbabilidad de frenado maxima en medicion con rango." },
    {SMART_MIN,  0,"", "smart_min", Arg::Required, "  \t--smart_min=<arg>  \tDensidad minima de autos inteligentes." },
    {SMART_MAX,  0,"", "smart_max", Arg::Required, "  \t--smart_max=<arg>  \tDensidad maxima de autos inteligentes." },
	{STOP_DENSITY,  0,"", "stop_density", Arg::Required, "  \t--stop_density=<arg>  \tDensidad de topes en ca_stop." },
    {NEW_CAR_MIN,  0,"", "new_car_min", Arg::Required, "  \t--new_car_min=<arg>  \tProbabilidad minima de nuevo auto en ac abierto." },
    {NEW_CAR_MAX,  0,"", "new_car_max", Arg::Required, "  \t--new_car_max=<arg>  \tProbabilidad maxima de nuevo auto en ac abierto." },
    {HELP,    0,"", "help", Arg::None,    "  \t--help  \tMuestra instrucciones." },
    {0,0,0,0,0,0}
};


int main(int argc, char* argv[])
{
    unsigned size = 100, iterations = 100;
    int vmax = 5, vmax_min = 0, vmax_max = 20;
    double density = 0.2, rand_prob = 0.2;
    double dt = 0.1, dmin = 0.0, dmax = 1.0, rand_prob_min = 0.0, rand_prob_max = 1.0;
    double smart_min = 0.0, smart_max = 0.7, new_car_min = 0.0, new_car_max = 1.0, stop_density = 0.1;
    bool measure_ocupancy = false, measure_flow = false, flow_vs_density = false;
    bool plot_traffic = false, flow_vs_vmax = false, flow_vs_rand_prob = false, flow_vs_smart_cars = false;
    bool flow_per_density = false, flow_vs_new_car = false;
    CA_TYPE ca_type = CIRCULAR_CA;
    double new_car_prob = 0.1, smart_density = 0.1;

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
            measure_ocupancy = true;
            break;

            case MEASURE_FLOW:
            measure_flow = true;
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

            case NEW_CAR_PROB:
            new_car_prob = aux_string_to_num<double>(opt.arg);
            break;

            case SMART_DENSITY:
            smart_density = aux_string_to_num<double>(opt.arg);
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

			case STOP_DENSITY:
			stop_density = aux_string_to_num<double>(opt.arg);
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
        }
    }

    delete[] options;
    delete[] buffer;

    // Verifica opciones.
    if ((measure_ocupancy || measure_flow) && (flow_vs_density || flow_vs_vmax || flow_vs_rand_prob || flow_vs_smart_cars || flow_vs_new_car))
    {
        cout << "Opciones incompatibles." << endl;
        return 1;
    }
    if (!(measure_ocupancy || measure_flow || flow_vs_density || flow_per_density || flow_vs_vmax 
		  || flow_vs_rand_prob || flow_vs_smart_cars || flow_vs_new_car))
        plot_traffic = true;    // Opcion predeterminada.
    if (flow_vs_vmax && dt < 1)
        dt = 1.0;

    double extra;	// Parámetro extra en el constructor de CA.
    if (ca_type == OPEN_CA)
        extra = new_car_prob;
    if (ca_type == SMART_CA)
        extra = smart_density;
	if (ca_type == STOP_CA)
		extra = stop_density;

    // Realiza acciones.
    if (measure_ocupancy || measure_flow || plot_traffic)
    {
        cout << "Evolucionando automata." << endl;
        CellularAutomata* ca = create_ca(ca_type, size, density, vmax, rand_prob, extra);
        ca->Evolve(iterations);
        if (plot_traffic)
        {
            cout << "Creando mapas." << endl;
            ca->DrawHistory();
            ca->DrawFlowHistory();
        }
        if (measure_ocupancy)
        {
            cout << "Midiendo ocupacion." << endl;
            single_measure_ocupancy(ca);
        }
        if (measure_flow)
        {
            cout << "Midiendo flujo." << endl;
            single_measure_flow(ca);
        }
        delete_ca();
    }
    else
    {
        if (flow_vs_density)
        {
            cout << "Midiendo flujo vs densidad." << endl;
            multiple_flow_vs_density(ca_type, size, iterations, vmax, dmin, dmax, dt, rand_prob, extra, false);
        }
        if (flow_per_density)
        {
            cout << "Midiendo flujo/densidad vs densidad." << endl;
            multiple_flow_vs_density(ca_type, size, iterations, vmax, dmin, dmax, dt, rand_prob, extra, true);
        }
        if (flow_vs_vmax)
        {
            cout << "Midiendo flujo vs vmax." << endl;
            multiple_flow_vs_vmax(ca_type, size, iterations, vmax_min, vmax_max, (int)dt, density, rand_prob, extra);
        }
        if (flow_vs_rand_prob)
        {
            cout << "Midiendo flujo vs rand_prob." << endl;
            multiple_flow_vs_rand_prob(ca_type, size, iterations, vmax, density, rand_prob_min, rand_prob_max, dt, extra);
        }
        if (flow_vs_smart_cars)
        {
            cout << "Midiendo flujo vs densidad de autos inteligentes." << endl;
            multiple_flow_vs_smart_cars(size, iterations, vmax, density, rand_prob, smart_min, smart_max, dt);
        }
        if (flow_vs_new_car)
        {
            cout << "Midiendo flujo vs probabilidad de nuevo auto." << endl;
            multiple_flow_vs_new_car_prob(size, iterations, vmax, density, rand_prob, new_car_min, new_car_max, dt);
        }
    }
    cout << "Hecho." << endl;
    return 0;
}