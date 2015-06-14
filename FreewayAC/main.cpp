#include <sstream>
#include <string>
#include "CellularAutomata.h"
#include "optionparser.h"
#include "Experiments.h"
using namespace std;


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

enum  OptionIndex { UNKNOWN, SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL,

                    PLOT_TRAFFIC, PLOT_FLOW, MEASURE_OCUPANCY, MEASURE_FLOW,
                    FLOW_VS_DENSITY, FLOW_PER_DENSITY, FLOW_VS_VMAX, FLOW_VS_RAND_PROB, FLOW_VS_AUT_CARS,
                    FLOW_VS_STOP_DENSITY, FLOW_VS_NEW_CAR, FLOW_PER_NEW_CAR, FLOW_VS_SEMAPHORE_DENSITY, ESCAPE_TIME_VS_DENSITY,
                    DISCHARGE_VS_DENSITY, DISCHARGE_VS_DENSITY_FRACTAL, DIMENSION_VS_DENSITY, DIMENSION_VS_DENSITY_THREADED,

                    CA_CIRCULAR, CA_MULTILANE_CIRCULAR, CA_OPEN, CA_MULTILANE_OPEN, CA_AUTONOMOUS, CA_STOP, CA_SEMAPHORE,
                    CA_SIMPLE_JUNCTION,

                    NEW_CAR_PROB, NEW_CAR_SPEED, AUT_DENSITY, STOP_DENSITY, SEMAPHORE_DENSITY, RANDOM_SEMAPHORES,
                    TARGET_LANE, LANES, RANDOM_SEED, RANDOM_GENERATOR, PARTITIONS,

                    DT, DMIN, DMAX, VMAX_MIN, VMAX_MAX, RAND_PROB_MIN, AUT_MIN, AUT_MAX,
                    STOP_DENSITY_MIN, STOP_DENSITY_MAX, NEW_CAR_MIN, NEW_CAR_MAX, RAND_PROB_MAX, SEMAPHORE_DENSITY_MIN,
                    SEMAPHORE_DENSITY_MAX,

                    OUT_FILE_NAME, PATH, EXPORT_FORMAT, SHOW_PROGRESS, TEST, DEBUG, HELP };

const option::Descriptor usage[] =
{
    {UNKNOWN, 0, "", "",Arg::None, "INSTRUCCIONES: FreewayAC [opciones]\n"},
    {SIZE,  0,"s", "size", Arg::Required, "  -s <arg>, \t--size=<arg>  \tTamagno del AC." },
    {ITERATIONS,  0,"i", "iter", Arg::Required, "  -i <arg>, \t--iter=<arg>  \tIteraciones del AC." },
    {VMAX,  0,"v", "vmax", Arg::Required, "  -v <arg>, \t--vmax=<arg>  \tVelocidad maxima de auto." },
    {DENSITY,  0,"d", "density", Arg::Required, "  -d <arg>, \t--density=<arg>  \tDensidad de autos." },
    {RAND_PROB,  0,"r", "rand_prob", Arg::Required, "  -r <arg>, \t--rand_prob=<arg>  \tProbabilidad de descenso de velocidad." },
	{INIT_VEL, 0, "i", "init_vel", Arg::Required, "  -i <arg>, \t--init_vel=<arg>  \tVelocidad inicial de los autos." },

    {PLOT_TRAFFIC,  0,"p","plot_traffic", Arg::None,
    "  -p , \t--plot_traffic  \tCrea mapa de posicion de autos vs tiempo." },
    {PLOT_FLOW, 0, "p", "plot_flow", Arg::None,
    "  -p , \t--plot_flow  \tCrea mapa de flujo de autos vs tiempo." },
    {MEASURE_OCUPANCY,  0,"o","ocupancy_fixed", Arg::None,
    "  -o , \t--ocupancy_fixed  \tMide la densidad de ocupacion promedio de cada casilla." },
    {MEASURE_FLOW,  0,"f","flow_fixed", Arg::None, "  -f , \t--flow_fixed  \tMide el flujo promedio de cada casilla." },
    {FLOW_VS_DENSITY,  0,"","flow_vs_density", Arg::None, 
    "  \t--flow_vs_density  \tMide el flujo respecto a densidad en un rango especificado por dmin, dmax y dt." },
    {FLOW_PER_DENSITY,  0,"","flow_per_density", Arg::None, 
    "  \t--flow_per_density  \tMide el flujo por densidad respecto a densidad en un rango especificado por dmin, dmax y dt." },
    {FLOW_VS_VMAX,  0,"","flow_vs_vmax", Arg::None, 
    "  \t--flow_vs_vmax  \tMide el flujo respecto a vmax en un rango especificado por vmax_min, vmax_max y dt." },
    {FLOW_VS_RAND_PROB,  0,"","flow_vs_rand_prob", Arg::None, 
    "  \t--flow_vs_rand_prob  \tMide el flujo respecto a probabilidad de frenado en un rango especificado por rand_min, rand_max y dt." },
    {FLOW_VS_AUT_CARS,  0,"","flow_vs_aut_cars", Arg::None,
    "  \t--flow_vs_aut_cars  \tMide el flujo respecto a densidad de vehiculos autonomos en un rango especificado por aut_min, aut_max y dt." },
    {FLOW_VS_STOP_DENSITY,  0,"","flow_vs_stop_density", Arg::None,
    "  \t--flow_vs_stop_density  \tMide el flujo respecto a densidad de topes en un rango especificado por stop_density_min, stop_density_max y dt." },
    {FLOW_VS_NEW_CAR,  0,"","flow_vs_new_car", Arg::None, 
    "  \t--flow_vs_new_car  \tFlujo respecto a probabilidad de nuevo auto en ac abierto en un rango especificado por new_car_min, new_car_max y dt." },
    {FLOW_PER_NEW_CAR, 0, "", "flow_per_new_car", Arg::None,
    "  \t--flow_per_new_car  \tFlujo/probabilidad respecto a probabilidad de nuevo auto en ac abierto." },
    {FLOW_VS_SEMAPHORE_DENSITY,  0,"","flow_vs_semaphore_density", Arg::None, 
    "  \t--flow_vs_semaphore_density  \tFlujo respecto a densidad de semaforos en un rango especificado por semaphore_density_min y semaphore_density_max." },
    {ESCAPE_TIME_VS_DENSITY,  0,"","escape_time_vs_density", Arg::None,
    "  \t--escape_time_vs_density  \tMide tiempo de escape vs densidad en un rango especificado por dmin, dmax y dt." },
    {DISCHARGE_VS_DENSITY,  0,"","discharge_vs_density", Arg::None,
    "  \t--discharge_vs_density  \tMide gasto vs densidad en un rango especificado por dmin, dmax y dt." },
    {DISCHARGE_VS_DENSITY_FRACTAL,  0,"","discharge_vs_density_fractal", Arg::None,
    "  \t--discharge_vs_density_fractal  \tCrea fractal de gasto vs densidad en un rango especificado por dmin, dmax y dt." },
    {DIMENSION_VS_DENSITY,  0,"","dimension_vs_density", Arg::None,
    "  \t--dimension_vs_density  \tMide dimension fractal vs densidad en un rango especificado por dmin, dmax y dt." },
    {DIMENSION_VS_DENSITY_THREADED, 0, "", "dimension_vs_density_threaded", Arg::None,
    "  \t--dimension_vs_density_threaded  \tMide dimension fractal vs densidad usando varios núcleos en un rango especificado por dmin, dmax y dt." },

    {CA_CIRCULAR,  0,"","ca_circular", Arg::None, "  \t--ca_circular  \tAutomata celular circular." },
    {CA_MULTILANE_CIRCULAR, 0, "", "ca_multilane_circular", Arg::None, 
    "  \t--ca_multilane_circular  \tAutomata celular multicarril circular." },
    {CA_OPEN,  0,"","ca_open", Arg::None, "  \t--ca_open  \tAutomata celular con frontera abierta." },
    {CA_MULTILANE_OPEN,  0,"","ca_multilane_open", Arg::None,
    "  \t--ca_multilane_open  \tAutomata celular multicarril con frontera abierta." },
    {CA_AUTONOMOUS,  0,"","ca_autonomous", Arg::None, "  \t--ca_autonomous  \tAutomata celular con vehiculos autonomos." },
    {CA_STOP,  0,"","ca_stop", Arg::None,
     "  \t--ca_stop  \tAutomata celular con tope. La posicion del tope se especifica por stop_density." },
    {CA_SEMAPHORE,  0,"","ca_semaphore", Arg::None, 
     "  \t--ca_semaphore  \tAutomata celular con semaforo. La posicion del semaforo se especifica por semaphore_density." },
    {CA_SIMPLE_JUNCTION, 0, "", "ca_simple_junction", Arg::None,
     "  \t--ca_simple_junction  \tAutomata de interseccion simple." },

    {NEW_CAR_PROB,  0,"","new_car_prob", Arg::Required, "  \t--new_car_prob  \tProbabilidad de que se aparezca nuevo auto en frontera abierta." },
    {NEW_CAR_SPEED, 0, "", "new_car_speed", Arg::Required, "  \t--new_car_speed  \tVelocidad que entre a AC abierto." },
    {AUT_DENSITY,  0,"","aut_density", Arg::Required, "  \t--aut_density  \tDensidad de autos autonomos." },
    {STOP_DENSITY,  0,"", "stop_density", Arg::Required, "  \t--stop_density=<arg>  \tDensidad de topes en ca_stop." },
    {SEMAPHORE_DENSITY,  0,"", "semaphore_density", Arg::Required, 
     "  \t--semaphore_density=<arg>  \tDensidad de semaforos en ca_semaphore." },
    {RANDOM_SEMAPHORES,  0,"", "random_semaphores", Arg::Required,
     "  \t--random_semaphores  \tTrue o False. Coloca los semaforos en posiciones aleatorias en vez de uniformes." },
    {TARGET_LANE, 0, "", "target_lane", Arg::Required,
     "  \t--target_lane  \tCarril objetivo en AC de uniones." },
    {LANES, 0, "", "lanes", Arg::Required,
     "  \t--lanes  \tNumero de carriles en AC multicarril." },
    {RANDOM_SEED, 0, "", "random_seed", Arg::Required,
    "  \t--random_seed  \tSemilla de generador de numeros aleatorios. Si no se especifica usa el tiempo del sistema." },
    {RANDOM_GENERATOR, 0, "", "random_generator", Arg::Required,
    "  \t--random_generator  \tTipo de generador de aletorios. Disponibles LCG, MT19937, RANLUX24, RANLUX48." },
    {PARTITIONS, 0, "", "partitions", Arg::Required,
    "  \t--partitions  \tNumero de particiones que se realizan sobre un intervalo para calcular dimension fractal." },

    {DT,  0,"", "dt", Arg::Required, "  \t--dt=<arg>  \tTamagno del intervalo en medicion con rango." },
    {DMIN,  0,"", "dmin", Arg::Required, "  \t--dmin=<arg>  \tDensidad minima en medicion con rango." },
    {DMAX,  0,"", "dmax", Arg::Required, "  \t--dmax=<arg>  \tDensidad maxima en medicion con rango." },
    {VMAX_MIN,  0,"", "vmax_min", Arg::Required, "  \t--vmax_min=<arg>  \tVmax minima en medicion con rango." },
    {VMAX_MAX,  0,"", "vmax_max", Arg::Required, "  \t--vmax_max=<arg>  \tVmax maxima en medicion con rango." },
    {RAND_PROB_MIN,  0,"", "rand_prob_min", Arg::Required, "  \t--rand_prob_min=<arg>  \tProbabilidad de frenado minima en medicion con rango." },
    {RAND_PROB_MAX,  0,"", "rand_prob_max", Arg::Required, "  \t--rand_prob_max=<arg>  \tProbabilidad de frenado maxima en medicion con rango." },
    {AUT_MIN,  0,"", "aut_min", Arg::Required, "  \t--aut_min=<arg>  \tDensidad minima de autos autonomos." },
    {AUT_MAX,  0,"", "aut_max", Arg::Required, "  \t--aut_max=<arg>  \tDensidad maxima de autos autonomos." },
    {STOP_DENSITY_MIN,  0,"", "stop_density_min", Arg::Required, "  \t--stop_density_min=<arg>  \tDensidad minima de topes." },
    {STOP_DENSITY_MAX,  0,"", "stop_density_max", Arg::Required, "  \t--stop_density_max=<arg>  \tDensidad maxima de topes." },
    {SEMAPHORE_DENSITY_MIN,  0,"", "semaphore_density_min", Arg::Required, "  \t--semaphore_density_min=<arg>  \tDensidad minima de semaforos." },
    {SEMAPHORE_DENSITY_MAX,  0,"", "semaphore_density_max", Arg::Required, "  \t--semaphore_density_max=<arg>  \tDensidad maxima de semaforos." },
    {NEW_CAR_MIN,  0,"", "new_car_min", Arg::Required, "  \t--new_car_min=<arg>  \tProbabilidad minima de nuevo auto en ac abierto." },
    {NEW_CAR_MAX,  0,"", "new_car_max", Arg::Required, "  \t--new_car_max=<arg>  \tProbabilidad maxima de nuevo auto en ac abierto." },

    {OUT_FILE_NAME,  0,"", "out_file_name", Arg::Required, "  \t--out_file_name=<arg>  \tCambia el nombre del archivo de salida al especificado." },
    {PATH,  0,"", "path", Arg::Required, "  \t--path=<arg>  \tRuta donde guardar archivos de salida." },
    {EXPORT_FORMAT, 0, "", "export_format", Arg::Required, "  \t--export_format=<arg>  \tFormato de salida. CSV o BMP." },
    {SHOW_PROGRESS,  0,"", "show_progress", Arg::Required, "  \t--show_progress=<arg>  \tTrue o False. Muestra o no barra de progreso." },
    {TEST,    0,"", "test", Arg::None,    "  \t--test  \tRealiza pruebas para garantizar la fiabilidad de resultados." },
    {DEBUG,    0,"", "debug", Arg::None,    "  \t--debug  \tModo de depuracion." },
    {HELP,    0,"", "help", Arg::None,    "  \t--help  \tMuestra instrucciones detalladas de cada experimento." },
    {0,0,0,0,0,0}
};

void describe_experiments()
{
    const char *text = "A continuacion se enumera la lista de experimentos y AC disponibles y sus parametros.\n"
        "Todas las opciones se representan en mayusculas.\n\n"
        "=== Tipos de automatas celulares ===\n"
        "CA_CIRCULAR           -> Descripcion: Automata celular con fronteras periodicas. Pista circular.\n"
        "                         Parametros relevantes: Ninguno.\n"
        "CA_MULTILANE_CIRCULAR -> Descripcion: Automata celular multicarril con fronteras periodicas.\n"
        "                         Parametros relevantes: LANES.\n"
        "CA_OPEN               -> Descripcion: Automata celular con fronteras abiertas. Entran autos en\n"
        "                                      la primera pos del AC.\n"
        "CA_MULTILANE_OPEN     -> Descripcion: Automata celular con fronteras abiertas. Entran autos en\n"
        "                          la primera pos del AC.\n"
        "                         Parametros relevantes: NEW_CAR_PROB, NEW_CAR_SPEED, LANES.\n"
        "CA_AUTONOMOUS         -> Descripcion: Automata celular circular con vehiculos autonomos.\n"
        "                         Parametros relevantes: AUT_DENSITY.\n"
        "CA_STOP               -> Descripcion: Automata celular circular con topes en la pista.\n"
        "                         Parametros relevantes: STOP_DENSITY.\n"
        "CA_SEMAPHORE          -> Descripcion: Automata celular circular con semaforos en la pista.\n"
        "                         Parametros relevantes: SEMAPHORE_DENSITY, RANDOM_SEMAPHORES.\n"
        "CA_SIMPLE_JUNCTION    -> Descripcion: Automata celular abierto con unión simple y fronteras abiertas.\n"
        "                         Parametros relevantes: NEW_CAR_PROB, NEW_CAR_SPEED, TARGET_LANE.\n"
        "\n=== Experimentos de parametro fijo ===\n"
        "PLOT_TRAFFIC          -> Descripcion: Evoluciona automata celular y grafica su representacion.\n"
        "                         Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL.\n"
        "PLOT_FLOW             -> Descripcion: Evoluciona automata celular y grafica su flujo.\n"
        "                         Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL.\n"
        "MEASURE_OCUPANCY      -> Descripcion: Mide la ocupacion de cada casilla del automata celular.\n"
        "                         Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL.\n"
        "MEASURE_FLOW          -> Descripcion: Mide el flujo de cada casilla del automata celular.\n"
        "                         Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL.\n"
        "\n=== Experimentos de parametro variable ===\n"
        "FLOW_VS_DENSITY       -> Descripcion: Mide el flujo de autos promedio respecto a los valores\n"
        "                                      de densidad de autos especificados en un rango.\n"
        "                         Parametros relevantes: SIZE, ITERATIONS, VMAX, RAND_PROB, INIT_VEL, DMIN, DMAX, DT.\n"
        "FLOW_PER_DENSITY      -> Descripcion: Mide el flujo de autos promedio sobre densidad respecto\n"
        "                                      a los valores de densidad de autos especificados en un rango.\n"
        "                         Parametros relevantes: SIZE, ITERATIONS, VMAX, RAND_PROB, INIT_VEL, DMIN, DMAX, DT.\n"
        "FLOW_VS_VMAX          -> Descripcion: Mide el flujo de autos promedio respecto a los valores de\n"
        "                                      velocidad maxima especificados en un rango.\n"
        "                         Parametros relevantes: SIZE, ITERATIONS, DENSITY, RAND_PROB, INIT_VEL, VMAX_MIN, VMAX_MAX, DT.\n"
        "FLOW_VS_RAND_PROB     -> Descripcion: Mide el flujo de autos promedio respecto a los valores de\n"
        "                                      probabilidad de descenso de velocidad especificados en un rango.\n"
        "                         Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, INIT_VEL, RAND_PROB_MIN,\n"
        "                                                RAND_PROB_MAX, DT.\n"
        "FLOW_VS_AUT_CARS      -> Descripcion: Mide el flujo de autos promedio respecto a los valores\n"
        "                                      de densidad de vehiculos autonomos especificados en un rango.\n"
        "                         Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL, \n"
        "                                                AUT_MIN, AUT_MAX, DT.\n"
        "FLOW_VS_STOP_DENSITY  -> Descripcion: Mide el flujo de autos promedio respecto a los valores\n"
        "                                      densidad de topes especificados en un rango. Se usa el\n"
        "                                      automata celular CA_STOP.\n"
        "                         Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL,\n"
        "                                                STOP_DENSITY_MIN, STOP_DENSITY_MAX, DT.\n"
        "FLOW_VS_NEW_CAR       -> Descripcion: Mide el flujo de autos promedio respecto a los valores\n"
        "                                      de probabilidad de aparicion de nuevo auto especificados\n"
        "                                      en un rango. Se usa el automata celular CA_OPEN o.\n"
        "                                      CA_SIMPLE_JUNCTION.\n"
        "                         Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL,\n"
        "                                                NEW_CAR_MIN, NEW_CAR_MAX, DT.\n"
        "FLOW_PER_NEW_CAR      -> Descripcion: Mide el flujo/prob de autos promedio respecto a los valores\n"
        "                                      de probabilidad de aparicion de nuevo auto especificados\n"
        "                                      en un rango. Se usa el automata celular CA_OPEN.\n"
        "                         Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL,\n"
        "                                                NEW_CAR_MIN, NEW_CAR_MAX, DT.\n"
        "FLOW_VS_SEMAPHORE_DENSITY  -> Descripcion: Mide el flujo de autos promedio respecto a los valores de\n"
        "                              densidad de semaforos especificados en un rango. Se usa el automata\n"
        "                              celular ca_semaphore.\n"
        "                              Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL,\n"
        "                                                     SEMAPHORE_DENSITY_MIN, SEMAPHORE_DENSITY_MIN,\n"
        "                                                     RANDOM_SEMAPHORES, DT.\n"
        "ESCAPE_TIME_VS_DENSITY     -> Descripcion: Mide el tiempo de escape respecto a los valores de\n"
        "                              densidad de vehiculos especificados en un rango. Se usa el automata\n"
        "                              celular ca_open.\n"
        "                              Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL,\n"
        "                                                     DMIN, DMAX, DT.\n"
        "DISCHARGE_VS_DENSITY       -> Descripcion: Mide el gasto respecto a los valores de\n"
        "                              densidad de vehiculos especificados en un rango. Se usa el automata\n"
        "                              celular ca_open.\n"
        "                              Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL,\n"
        "                                                     DMIN, DMAX, DT.\n"
        "DISCHARGE_VS_DENSITY_FRACTAL -> Descripcion: Mide la dimension fractal en un mapa creado por los valores de\n"
        "                                densidad de vehiculos especificados en un rango. Se usa el automata\n"
        "                                celular ca_open.\n"
        "                                Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL,\n"
        "                                                       DMIN, DMAX, DT.\n"
        "DIMENSION_VS_DENSITY      -> Descripcion: Crea grafica de dimension fractal respecto a los valores de\n"
        "                              densidad de vehículos especificados en un rango. Se usa el automata\n"
        "                              celular ca_open.\n"
        "                              Parametros relevantes: SIZE, ITERATIONS, VMAX, DENSITY, RAND_PROB, INIT_VEL,\n"
        "                                                     DMIN, DMAX, DT, PARTITIONS.\n";
    cout << text << endl;
}

int main(int argc, char* argv[])
{
    unsigned size = 100, iterations = 100;
    int vmax = 5, vmax_min = 0, vmax_max = 20, init_vel = 1;
    double density = 0.2, rand_prob = 0.2;

    bool ocupancy_fixed = false, flow_fixed = false, flow_vs_density = false;
    bool plot_traffic = false, plot_flow = false, flow_vs_vmax = false, flow_vs_rand_prob = false, flow_vs_aut_cars = false;
    bool flow_vs_stop_density = false, flow_per_density = false, flow_vs_new_car = false, flow_per_new_car = false;
    bool flow_vs_semaphore_density = false, escape_time_vs_density = false, discharge_vs_density = false;
    bool discharge_vs_density_fractal = false, dimension_vs_density = false, dimension_vs_density_threaded = false;
    bool per_density = false, per_prob = false;

    bool random_semaphores = false, test = false, show_progress = true;

    double dt = 0.1, dmin = 0.0, dmax = 1.0, rand_prob_min = 0.0, rand_prob_max = 1.0;
    double aut_min = 0.0, aut_max = 1.0, new_car_min = 0.0, new_car_max = 1.0, stop_density_min = 0.0;
    double stop_density_max = 1.0, semaphore_density_min = 0.0, semaphore_density_max = 1.0;

    bool multilane = false;
    bool debug = false;
    CA_TYPE ca_type = CIRCULAR_CA;
    double new_car_prob = 0.1, aut_density = 0.1, stop_density = 0.1, semaphore_density = 0.1;
    int new_car_speed = 1, target_lane = 0, lanes = 2, random_seed = -1, partitions = 50;
    string random_generator = "", export_format = "", out_file_name = "", path = "";

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

            case PLOT_FLOW:
            plot_flow = true;
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

            case FLOW_VS_AUT_CARS:
            flow_vs_aut_cars = true;
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

            case FLOW_PER_NEW_CAR:
            flow_per_new_car = true;
            break;

            case ESCAPE_TIME_VS_DENSITY:
            escape_time_vs_density = true;
            break;

            case DISCHARGE_VS_DENSITY:
            discharge_vs_density = true;
            break;

            case DISCHARGE_VS_DENSITY_FRACTAL:
            discharge_vs_density_fractal = true;
            break;

            case DIMENSION_VS_DENSITY:
            dimension_vs_density = true;
            break;

            case DIMENSION_VS_DENSITY_THREADED:
            dimension_vs_density_threaded = true;
            break;

            case CA_CIRCULAR:
            ca_type = CIRCULAR_CA;
            break;

            case CA_MULTILANE_CIRCULAR:
            ca_type = CIRCULAR_MULTILANE_CA;
            break;

            case CA_OPEN:
            ca_type = OPEN_CA;
            break;

            case CA_MULTILANE_OPEN:
            ca_type = OPEN_MULTILANE_CA;
            break;

            case CA_AUTONOMOUS:
            ca_type = AUTONOMOUS_CA;
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

            case AUT_DENSITY:
            aut_density = aux_string_to_num<double>(opt.arg);
            break;

            case STOP_DENSITY:
            stop_density = aux_string_to_num<double>(opt.arg);
            break;

            case SEMAPHORE_DENSITY:
            semaphore_density = aux_string_to_num<double>(opt.arg);
            break;

            case RANDOM_SEMAPHORES:
            random_semaphores = aux_string_to_bool(opt.arg);
            break;

            case TARGET_LANE:
            target_lane = aux_string_to_num<int>(opt.arg);
            break;

            case LANES:
            lanes = aux_string_to_num<int>(opt.arg);
            break;

            case RANDOM_SEED:
            random_seed = aux_string_to_num<int>(opt.arg);
            break;

            case RANDOM_GENERATOR:
            random_generator = opt.arg;
            break;

            case PARTITIONS:
            partitions = aux_string_to_num<int>(opt.arg);
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

            case AUT_MIN:
            aut_min = aux_string_to_num<double>(opt.arg);
            break;

            case AUT_MAX:
            aut_max = aux_string_to_num<double>(opt.arg);
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

            case PATH:
            path = opt.arg;
            break;

            case EXPORT_FORMAT:
            export_format = opt.arg;
            break;

            case SHOW_PROGRESS:
            show_progress = aux_string_to_bool(opt.arg);
            break;

            case TEST:
            test = true;
            break;

            case DEBUG:
            debug = true;
            break;
        }
    }

    delete[] options;
    delete[] buffer;

    if (debug)
    {
        aux_create_directory("Carpeta");
        return 0;
    }

    // Verifica opciones.
    if (!path.empty() && !df_directory_exist(path))
    {
        cout << "Path no valido." << endl;
        return 1;
    }

    if(random_generator == "LCG")
        RandomGen::SetAlgorithm(LCG);
    else if (random_generator == "MT19937")
        RandomGen::SetAlgorithm(MT19937);
    else if (random_generator == "RANLUX24")
        RandomGen::SetAlgorithm(RANLUX24);
    else if (random_generator == "RANLUX48")
        RandomGen::SetAlgorithm(RANLUX48);
    else
        RandomGen::SetAlgorithm(MT19937);

    if (!(ocupancy_fixed || flow_fixed || flow_vs_density || flow_per_density || flow_vs_vmax 
        || flow_vs_rand_prob || flow_vs_aut_cars || flow_vs_new_car || flow_per_new_car
        || flow_vs_stop_density || flow_vs_semaphore_density || escape_time_vs_density
        || discharge_vs_density || discharge_vs_density_fractal || dimension_vs_density 
        || dimension_vs_density_threaded || test))
    {
        plot_traffic = true;    // Opcion predeterminada.
    }
    if (flow_per_density)
        per_density = true;
    if (flow_per_new_car)
        per_prob = true;
    if (flow_vs_vmax && dt < 1)
        dt = 1.0;

    Args args;            // Parámetro extra en el constructor de CA.
    if (ca_type == OPEN_CA)
        args = Args({ new_car_prob }, { new_car_speed });
    if (ca_type == AUTONOMOUS_CA)
        args = Args({ aut_density });
    if (ca_type == STOP_CA)
        args = Args({ stop_density });
    if (ca_type == SEMAPHORE_CA)
        args = Args({ semaphore_density }, {}, { random_semaphores });
    if (ca_type == SIMPLE_JUNCTION_CA)
        args = Args({ new_car_prob }, { new_car_speed, target_lane });
    if (ca_type == CIRCULAR_MULTILANE_CA)
        multilane = true;
    if (ca_type == OPEN_MULTILANE_CA)
    {
        multilane = true;
        args = Args({ new_car_prob }, { new_car_speed });
    }

    ExParam param;
    param.type = ca_type;
    param.size = size;
    param.iterations = iterations;
    param.lanes = lanes;
    param.vmax = vmax;
    param.vmax_min = vmax_min;
    param.vmax_max = vmax_max;
    param.random_seed = random_seed;
    param.partitions = partitions;
    param.density = density;
    param.density_min = dmin;
    param.density_max = dmax;
    param.rand_prob = rand_prob;
    param.rand_prob_min = rand_prob_min;
    param.rand_prob_max = rand_prob_max;
    param.aut_car_density_min = aut_min;
    param.aut_car_density_max = aut_max;
    param.new_car_prob_min = new_car_min;
    param.new_car_prob_max = new_car_max;
    param.stop_density_min = stop_density_min;
    param.stop_density_max = stop_density_max;
    param.semaphore_density_min = semaphore_density_min;
    param.semaphore_density_max = semaphore_density_max;
    param.per_density = per_density;
    param.per_prob = per_prob;
    param.random_semaphores = random_semaphores;
    param.show_progress = show_progress;
    param.path = path;
    param.out_file_name = out_file_name;
    param.args = args;
    param.dt = dt;

    if (!export_format.empty())
    {
        if (export_format == "CSV")
            param.export_format = CSV;
        else if (export_format == "BMP")
            param.export_format = BMP;
    }
    else
        param.export_format = CSV;

    // Realiza acciones.
    if (test)
    {
        cout << "Realizando tests." << endl;
        ex_perform_test();
    }

    int r = 0;
    if (!multilane)
    {
        if (plot_traffic)
        {
            cout << "Creando mapa de trafico." << endl;
            r = ex_traffic_map(param);
        }
        if (ocupancy_fixed)
        {
            cout << "Midiendo ocupacion." << endl;
            r = ex_ocupancy_fixed(param);
        }
        if (flow_fixed)
        {
            cout << "Midiendo flujo." << endl;
            r = ex_flow_fixed(param);
        }
        if (flow_vs_density)
        {
            cout << "Midiendo flujo vs densidad." << endl;
            r = ex_flow_vs_density(param);
        }
        if (flow_per_density)
        {
            cout << "Midiendo flujo/densidad vs densidad." << endl;
            r = ex_flow_vs_density(param);
        }
        if (flow_vs_vmax)
        {
            cout << "Midiendo flujo vs vmax." << endl;
            r = ex_flow_vs_vmax(param);
        }
        if (flow_vs_rand_prob)
        {
            cout << "Midiendo flujo vs rand_prob." << endl;
            r = ex_flow_vs_rand_prob(param);
        }
        if (flow_vs_aut_cars)
        {
            cout << "Midiendo flujo vs densidad de autos inteligentes." << endl;
            r = ex_flow_vs_aut_cars(param);
        }
        if (flow_vs_stop_density)
        {
            cout << "Midiendo flujo vs densidad de topes." << endl;
            r = ex_flow_vs_stop_density(param);
        }
        if (flow_vs_new_car)
        {
            cout << "Midiendo flujo vs probabilidad de nuevo auto." << endl;
            r = ex_flow_vs_new_car_prob(param);
        }
        if (flow_per_new_car)
        {
            cout << "Midiendo flujo/probabilidad vs probabilidad de nuevo auto." << endl;
            r = ex_flow_vs_new_car_prob(param);
        }
        if (flow_vs_semaphore_density)
        {
            cout << "Midiendo flujo vs densidad de semaforos." << endl;
            r = ex_flow_vs_semaphore_density(param);
        }
        if (escape_time_vs_density)
        {
            cout << "Midiendo tiempo de escape vs densidad de autos." << endl;
            r = ex_escape_time_vs_density(param);
        }
        if (discharge_vs_density)
        {
            cout << "Midiendo gasto vs densidad de autos." << endl;
            r = ex_discharge_vs_density(param);
        }
        if (discharge_vs_density_fractal)
        {
            cout << "Creando fractal de gasto vs densidad de autos." << endl;
            r = ex_discharge_vs_density_fratal(param);
        }
        if (dimension_vs_density)
        {
            cout << "Midiendo dimension fractal vs densidad de autos." << endl;
            r = ex_dimension_vs_density(param);
        }
        if (dimension_vs_density_threaded)
        {
            cout << "Midiendo dimension fractal vs densidad de autos." << endl;
            r = ex_dimension_vs_density_parallel(param);
        }
    }
    else
    {
        if (plot_traffic)
        {
            cout << "Evolucionando automata." << endl;
            CaHandlerML ca(ca_type, size, lanes, density, vmax, rand_prob, init_vel, args);
            ca.Evolve(iterations);
            if (plot_traffic)
            {
                cout << "Creando mapas." << endl;
                r = ca.DrawHistory(path, out_file_name);
            }
        }
        else
        {
            if (flow_vs_density)
            {
                cout << "Midiendo flujo vs densidad." << endl;
                r = ex_multilane_flow_vs_density(param);
            }
            if (flow_per_density)
            {
                cout << "Midiendo flujo por densidad." << endl;
                r = ex_multilane_flow_vs_density(param);
            }
        }
    }
    if (r == 0)
        cout << "Hecho." << endl;
    else
        cout << "Error en la ejecucion." << endl;

    return 0;
}
