/**
* @file CellularAutomata.h
* @brief Clases de autómatas celulares.
* @author Carlos Manuel Rodríguez Martínez
* @date 18/05/2015
*/

#ifndef _CELLULARAUTOMATA
#define _CELLULARAUTOMATA

#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include "BmpWriter.h"
#include "Auxiliar.h"


////////////////////////////////////
//                                //
//        Autómatas celulares     //
//        de carril único         //
//                                //
////////////////////////////////////

/****************************
*                           *
*         AC Básico         *
*                           *
****************************/

/**
* @enum CAS
* @brief Enumera los tipos de listas de AC.
*
* Se usan diferentes tipos de listas para guardar información del autómata celular.
*/
enum CAS
{
    CA, CA_TEMP, CA_FLOW_TEMP, CA_HISTORY, CA_FLOW_HISTORY
};


/**
* @enum CA_TYPE
* @brief Enumera los tipos de autómatas celulares disponibles.
*/
enum CA_TYPE
{
    CIRCULAR_CA, CIRCULAR_MULTILANE_CA, OPEN_CA, OPEN_MULTILANE_CA, AUTONOMOUS_CA, STOP_CA,
    SEMAPHORE_CA, SIMPLE_JUNCTION_CA
};

typedef int CaPosition;
typedef unsigned CaLane;

/**
 * @class CellularAutomata
 * @brief Clase base para autómata celular.
 * Esta clase implementa los métodos básicos que todos los autómatas celulares de tráfico poseen.
 * Las condiciones de frontera del autómata se reflejan en los métodos At y Move que se necesitan
 * definir en las clases hijas.
 */
class CellularAutomata
{
protected:
    bool m_test;                 ///< Modo de prueba.
    int m_vmax;                  ///< Valor máximo de la velocidad.
    double m_rand_prob;          ///< Valor de la probabilidad de descenso de velocidad.
    int m_init_vel;              ///< Velocidad inicial de los autos.
    unsigned m_size;             ///< Tamaño del autómata celular
    std::vector<int> m_ca;       ///< Automata celular. -1 para casillas sin auto, y valores >= 0 indican velocidad del auto en esa casilla.
    std::vector<int> m_ca_temp, m_ca_flow_temp;                         ///< Variable temporal para operaciones con AC.
    std::vector< std::vector<int> > m_ca_history, m_ca_flow_history;    ///< Lista con valores históricos de AC.
    std::vector<bool> m_rand_values;                                    ///< Lista con valores aleatorios para usar en modo de prueba.

    // Conexión de carriles.
    CellularAutomata* m_connect;  ///< Puntero al AC al cual se va a conectar.
    CaPosition m_connect_pos;     ///< Posición del AC objetivo donde se realiza la conexión.

public:
    ///@brief Constructor.
    ///@param size Tamaño del AC.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param rand_prob Probabilidad de descenso de velocidad.
    CellularAutomata(const unsigned size, const double density, const int vmax, const double rand_prob, const int init_vel);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    CellularAutomata(const std::vector<int> &ca, const std::vector<bool> &rand_values, const int vmax);

    virtual ~CellularAutomata();

    ///@brief Evoluciona (itera) el AC.
    ///@param iter Número de iteraciones.
    virtual void Evolve(const unsigned iter);

    ///@brief Devuelve la distancia al auto más próximo desde la posición pos.
    ///@param pos Posición desde dónde iniciar la búsqueda.
    int NextCarDist(const CaPosition pos) const;

    ///@brief Devuelve valores verdaderos con probabilidad prob. Si se usa en prueba usa valores de lista.
    ///@param prob Probabilidad de obtener valor verdadero. Por defecto se utiliza m_rand_prob.
    bool Randomization(const double prob = -1.0);

    ///@brief Devuelve referencia a elemento del AC considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    virtual int &At(const CaPosition i) = 0;
    virtual int &AtTemp(const CaPosition i) = 0;
    virtual int &AtFlowTemp(const CaPosition i) = 0;

    ///@brief Devuelve elemento del AC.
    ///@param i Posición dentro del AC.
    int GetAt(const CaPosition i) const;

    ///@brief Devuelve referencia a elemento del AC en conexión.
    ///@param i Posición dentro del AC.
    int &AtConnected(const CaPosition i);

    ///@brief Conecta AC con otro. El flujo de autos ocurre desde el que realiza la conexión al objetivo.
    ///@param connect Puntero a AC objetivo.
    ///@param connect_pos Posición del AC objetivo donde se realiza la conexión.
    void Connect(CellularAutomata* connect, const CaPosition from, const CaPosition to);

    ///@brief Dibuja mapa histórico del AC en formato BMP.
    ///@param path Ruta del archivo.
    ///@param out_file_name Nombre del archivo de salida.
    ///@return 0 si se pudo crear archivo, 1 en caso de error.
    virtual int DrawHistory(std::string path = "", std::string out_file_name = "") const;

    ///@brief Dibuja mapa histórico del flujo de AC en formato BMP.
    ///@param path Ruta del archivo.
    ///@param out_file_name Nombre del archivo de salida.
    ///@return 0 si se pudo crear archivo, 1 en caso de error.
    virtual int DrawFlowHistory(std::string path = "", std::string out_file_name = "") const;
    
    std::vector<double> CalculateOcupancy() const;
    std::vector<double> CalculateFlow() const;
    double CalculateMeanFlow() const;

    void Print() const;                   ///< Escribe línea de autómata celular en la terminal.
    unsigned GetSize() const;             ///< Devuelve tamaño del AC.
    unsigned GetHistorySize() const;      ///< Devuelve tamaño de la lista histórica de evolución del AC.
    unsigned CountCars() const;           ///< Cuenta la cantidad de autos en AC.
    bool IsFluxHalted() const;            ///< Informa si el flujo está estancado.
    void PrintHistory() const;            ///< Escribe los valores históricos del AC en la terminal.
    virtual void Step();            ///< Aplica reglas de evolución temporal del AC.
    virtual void Move();            ///< Mueve los autos según las condiciones de frontera especificadas en clase hija.
    void AssignChanges();           ///< Asigna cambios de los arrays teporales al array m_ca e historico.
};


/****************************
*                           *
*        AC Circular        *
*                           *
****************************/

/**
 * @class CircularCA
 * @brief AC con condiciones de frontera periódicas.
 */
class CircularCA : public CellularAutomata
{
public:
    ///@brief Constructor.
    ///@param size Tamaño del AC.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param rand_prob Probabilidad de descenso de velocidad.
    CircularCA(const unsigned size, const double density, const int vmax, const double rand_prob, const int init_vel);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    CircularCA(const std::vector<int> &ca, const std::vector<bool> &rand_values, const int vmax);

    ///@brief Devuelve elemento de valores del autómata celular considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    int &At(const CaPosition i);
    int &AtTemp(const CaPosition i);
    int &AtFlowTemp(const CaPosition i);

    ///@brief Evoluciona (itera) el AC. Verifica si se conserva la cantidad de autos.
    ///@param iter Número de iteraciones.
    void Evolve(const unsigned iter);
};


/****************************
*                           *
*        AC Abierto         *
*                           *
****************************/

/**
 * @class OpenCA
 * @brief AC con condiciones de frontera abiertas.
 */
class OpenCA : public CellularAutomata
{
protected:
    int m_ca_empty;            ///< Se usa para devolver referencia de lugar vacío.
    int m_ca_flow_empty;
    double m_new_car_prob;  ///< Probabilidad de que aparezca un nuevo auto en la posición 0 del AC en la siguiente iteración.
    int m_new_car_speed;    ///< Velocidad de nuevo auto cuando ingresa a la pista.
public:
    ///@brief Constructor.
    ///@param size Tamaño del AC.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param rand_prob Probabilidad de descenso de velocidad.
    ///@param new_car_prob Probabilidad de que aparezca un nuevo auto en la posición 0 del AC en la siguiente iteración.
    ///@param new_car_speed Velocidad de nuevo auto cuando ingresa a la pista.
    OpenCA(const unsigned size, const double density, const int vmax, const double rand_prob, const int init_vel,
           const double new_car_prob, const int new_car_speed);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param new_car_speed Velocidad de nuevo auto cuando ingresa a la pista.
    OpenCA(const std::vector<int> &ca, const std::vector<bool> &rand_values, const int vmax, const int new_car_speed);

    ///@brief Devuelve elemento de valores del autómata celular considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    int &At(const CaPosition i);
    int &AtTemp(const CaPosition i);
    int &AtFlowTemp(const CaPosition i);

    void Step();    ///< Aplica reglas de evolución temporal del AC.
};


/****************************
*                           *
*    AC Coches autónomos    *
*                           *
****************************/

/**
 * @class AutonomousCA
 * @brief AC con autos autónomos y condiciones de frontera periódicas.
 */
class AutonomousCA : public CircularCA
{
protected:
    std::vector<int> m_aut_cars;  ///< Lista con posiciones de autos autónomos.
public:
    ///@brief Constructor.
    ///@param size Tamaño del AC.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param rand_prob Probabilidad de descenso de velocidad.
    ///@param aut_density Densidad de autos autónomos respecto a número total de autos.
    AutonomousCA(const unsigned size, const double density, const int vmax, const double rand_prob,
                 const int init_vel, const double aut_density);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param aut_cars Lista con posiciones de autos autónomos.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    AutonomousCA(const std::vector<int> &ca, std::vector<int> &aut_cars, const std::vector<bool> &rand_values,
                 const int vmax);

    void Move();    ///< Mueve los autos con condiciones de frontera periódicas.
    void Step();    ///< Aplica reglas de evolución temporal del AC para autos normales e inteligentes.
};

/****************************
*                           *
*          AC Tope          *
*                           *
****************************/

/**
 * @class StreetStopCA
 * @brief AC con topes y condiciones de frontera periódicas.
 */
class StreetStopCA : public CircularCA
{
protected:
    std::vector<unsigned> m_stop_pos;   ///< Lista con posiciones de los topes.
public:
    ///@brief Constructor.
    ///@param size Tamaño del AC.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param rand_prob Probabilidad de descenso de velocidad.
    ///@param stop_density Densidad de topes respecto a tamaño total del AC.
    StreetStopCA(const unsigned size, const double density, const int vmax, const double rand_prob,
                 const int init_vel, const double stop_density);

    ///@brief Devuelve la distancia al tope más próximo desde la posición pos.
    ///@param pos Posición desde dónde iniciar la búsqueda.
    int NextStopDist(const CaPosition pos) const;

    ///@brief Dibuja mapa histórico del AC en formato BMP.
    ///@param path Ruta del archivo.
    ///@param out_file_name Nombre del archivo de salida.
    ///@return 0 si se pudo crear archivo, 1 en caso de error.
    int DrawHistory(std::string path = "", std::string out_file_name = "") const;

    void Step();        ///< Aplica reglas de evolución temporal del AC con tope.
};

/****************************
*                           *
*        AC Semáforo        *
*                           *
****************************/

/**
 * @class SemaphoreCA
 * @brief AC con semáforos y condiciones de frontera periódicas.
 */
class SemaphoreCA : public CircularCA
{
protected:
    std::vector<CaPosition> m_semaphore_pos;              ///< Lista con posiciones de los semáforos.
    std::vector<int> m_semaphore_value;                   ///< Valor inicial del tiempo del semáforo.
    std::vector< std::vector<int> > m_semaphore_val_hist; ///< Valores historicos de valores de semaforo.
    int m_semaphore_init;                                 ///< Valor del contador de semáforo con el cual comienza.
    int m_semaphore_open;                                 ///< Valor del contador del semáforo en el cual se abre.
public:
    ///@brief Constructor.
    ///@param size Tamaño del AC.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param rand_prob Probabilidad de descenso de velocidad.
    ///@param semaphore_density Densidad de semaforos respecto a tamaño total del AC.
    SemaphoreCA(const unsigned size, const double density, const int vmax, const double rand_prob,
                const int init_vel, const double semaphore_density, const bool random_semaphores = false);

    ///@brief Devuelve la distancia al semáforo más próximo desde la posición pos.
    ///@param pos Posición desde dónde iniciar la búsqueda.
    int NextSemaphoreDist(const CaPosition pos) const;

    ///@brief Dibuja mapa histórico del AC en formato BMP.
    ///@param path Ruta del archivo.
    ///@param out_file_name Nombre del archivo de salida.
    ///@return 0 si se pudo crear archivo, 1 en caso de error.
    int DrawHistory(std::string path = "", std::string out_file_name = "") const;

    void Step();        ///< Aplica reglas de evolución temporal del AC con tope.
};

/****************************
*                           *
*  AC Intersección simple   *
*                           *
****************************/

/**
* @class SimpleJunctionCA
* @brief AC Con intersección simple y fronteras abiertas.
*/
class SimpleJunctionCA : public OpenCA
{
protected:
    OpenCA *m_source;
    int m_target_lane;
public:
    ///@brief Constructor.
    ///@param size Tamaño del AC.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param rand_prob Probabilidad de descenso de velocidad.
    ///@param return_lane Carril al que apunta la función At.
    SimpleJunctionCA(const unsigned size, const double density, const int vmax, const double rand_prob,
                     const int init_vel, const double new_car_prob, const int new_car_speed, const int target_lane = 0);

    ~SimpleJunctionCA();

    ///@brief Evoluciona (itera) el AC. Verifica si se conserva la cantidad de autos.
    ///@param iter Número de iteraciones.
    void Evolve(const unsigned iter);

    ///@brief Dibuja mapa histórico del AC en formato BMP.
    ///@param path Ruta del archivo.
    ///@param out_file_name Nombre del archivo de salida.
    ///@return 0 si se pudo crear archivo, 1 en caso de error.
    int DrawHistory(std::string path = "", std::string out_file_name = "") const;
};


////////////////////////////////////
//                                //
//      Autómatas celulares       //
//      de varios carriles        //
//                                //
////////////////////////////////////

class CAElement : public std::vector<int>
{
public:
    CAElement(const int lanes, const int def_val = -1);
};

/****************************
*                           *
*   AC Básico Multicarril   *
*                           *
****************************/

/**
* @class CellularAutomataML
* @brief Clase base para autómata celular multicarril.
* Esta clase implementa los métodos básicos que todos los autómatas celulares de tráfico poseen.
* Las condiciones de frontera del autómata se reflejan en los métodos At y Move que se necesitan
* definir en las clases hijas.
*/

class CellularAutomataML
{
protected:
    bool m_test;                 ///< Modo de prueba.
    int m_vmax;                  ///< Valor máximo de la velocidad.
    unsigned m_lanes;            ///< Número de carriles.
    double m_rand_prob;          ///< Valor de la probabilidad de descenso de velocidad.
    int m_init_vel;              ///< Velocidad inicial de los vehículos.
    unsigned m_size;             ///< Tamaño del autómata celular
    std::vector<CAElement> m_ca;    ///< Automata celular. -1 para casillas sin auto, y valores >= 0 indican velocidad del auto en esa casilla.
    std::vector<CAElement> m_ca_temp, m_ca_flow_temp;                         ///< Variable temporal para operaciones con AC.
    std::vector< std::vector<CAElement> > m_ca_history, m_ca_flow_history;    ///< Lista con valores históricos de AC.
    std::vector<bool> m_rand_values;                                    ///< Lista con valores aleatorios para usar en modo de prueba.

    // Conexión de carriles.
    CellularAutomataML* m_connect; ///< Puntero al AC al cual se va a conectar.
    CaPosition m_connect_pos;        ///< Posición del AC objetivo donde se realiza la conexión.

public:
    ///@brief Constructor.
    ///@param size Tamaño del AC.
    ///@param lanes Número de carriles.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param rand_prob Probabilidad de descenso de velocidad.
    CellularAutomataML(const unsigned size, const unsigned lanes, const double density, 
                       const int vmax, const double rand_prob, const int init_vel);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    CellularAutomataML(const std::vector<CAElement> &ca, const std::vector<bool> &rand_values,
                       const int vmax);

    virtual ~CellularAutomataML();

    ///@brief Evoluciona (itera) el AC.
    ///@param iter Número de iteraciones.
    void Evolve(const unsigned iter);

    ///@brief Devuelve referencia a  elemento de valores del autómata celular considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    ///@param lane Carril objetivo.
    virtual int &At(const CaPosition i, const CaLane lane) = 0;
    virtual int &AtTemp(const CaPosition i, const CaLane lane) = 0;
    virtual int &AtFlowTemp(const CaPosition i, const CaLane lane) = 0;

    ///@brief Similar a AC pero sólo devuelve el valor y en AC de uniones puede apuntar a un carril en específico.
    ///@param i Posición dentro del AC.
    ///@param ca Tipo de AC.
    int GetAt(const CaPosition i, const CaLane lane) const;

    ///@brief Conecta AC con otro. El flujo de autos ocurre desde el que realiza la conexión al objetivo.
    ///@param connect Puntero a AC objetivo.
    ///@param connect_pos Posición del AC objetivo donde se realiza la conexión.
    void Connect(CellularAutomataML* connect, CaPosition connect_pos);

    ///@brief Devuelve la distancia al auto más próximo desde la posición pos.
    ///@param pos Posición desde dónde iniciar la búsqueda.
    virtual int NextCarDist(const CaPosition pos, const CaLane lane) const;

    ///@brief Dibuja mapa histórico del AC en formato BMP.
    ///@param path Ruta del archivo.
    ///@param out_file_name Nombre del archivo de salida.
    ///@return 0 si se pudo crear archivo, 1 en caso de error.
    int DrawHistory(std::string path = "", std::string out_file_name = "") const;

    ///@brief Dibuja mapa histórico del flujo de AC en formato BMP.
    ///@param path Ruta del archivo.
    ///@param out_file_name Nombre del archivo de salida.
    ///@return 0 si se pudo crear archivo, 1 en caso de error.
    int DrawFlowHistory(std::string path = "", std::string out_file_name = "") const;

    ///@brief Devuelve valores verdaderos con probabilidad prob. Si se usa en prueba usa valores de lista.
    ///@param prob Probabilidad de obtener valor verdadero. Por defecto se utiliza m_rand_prob.
    bool Randomization(const double prob = -1.0);

    std::vector<double> CalculateOcupancy() const;
    std::vector<double> CalculateFlow() const;
    double CalculateMeanFlow() const;

    void Print() const;               ///< Escribe línea de autómata celular en la terminal.
    unsigned GetSize() const;         ///< Devuelve tamaño del AC.
    unsigned GetHistorySize() const;  ///< Devuelve tamaño de la lista histórica de evolución del AC.
    unsigned GetLanes() const;        ///< Devuelve el número de carriles.
    unsigned CountCars() const;       ///< Cuenta la cantidad de autos en AC.
    bool IsFluxHalted() const;        ///< Informa si el flujo está estancado.
    void PrintHistory() const;        ///< Escribe los valores históricos del AC en la terminal.
    virtual void Step();        ///< Aplica reglas de evolución temporal del AC.
    virtual void ChangeLanes(); ///< Aplica las reglas de cambio carril.
    virtual void Move();        ///< Mueve los autos según las condiciones de frontera especificadas en clase hija.
    void AssignChanges();           ///< Asigna cambios de los arrays teporales al array m_ca e historico.
};

/****************************
*                           *
*      AC Circular ML       *
*                           *
****************************/

/**
* @class CircularCAML
* @brief AC multicarril con condiciones de frontera periódicas.
*/

class CircularCAML : public CellularAutomataML
{
public:
    ///@brief Constructor.
    ///@param size Tamaño del AC.
    ///@param lanes Número de carriles.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param rand_prob Probabilidad de descenso de velocidad.
    CircularCAML(const unsigned size, const unsigned int lanes, const double density,
                 const int vmax, const double rand_prob, const int init_vel);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    CircularCAML(const std::vector<CAElement> &ca, const std::vector<bool> &rand_values, const int vmax);

    ///@brief Devuelve referencia a  elemento de valores del autómata celular considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    ///@param lane Carril objetivo.
    int &At(const CaPosition i, const CaLane lane);
    int &AtTemp(const CaPosition i, const CaLane lane);
    int &AtFlowTemp(const CaPosition i, const CaLane lane);

    ///@brief Evoluciona (itera) el AC. Verifica si se conserva la cantidad de autos.
    ///@param iter Número de iteraciones.
    void Evolve(const unsigned iter);
};


/****************************
*                           *
*       AC Abierto ML       *
*                           *
****************************/

/**
 * @class OpenCAML
 * @brief AC multicarril con condiciones de frontera abiertas.
 */
class OpenCAML : public CellularAutomataML
{
protected:
    int m_ca_empty;         ///< Se usa para devolver referencia de lugar vacío.
    int m_ca_flow_empty;
    double m_new_car_prob;  ///< Probabilidad de que aparezca un nuevo auto en la posición 0 del AC en la siguiente iteración.
    int m_new_car_speed;    ///< Velocidad de nuevo auto cuando ingresa a la pista.
public:
    ///@brief Constructor.
    ///@param size Tamaño del AC.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param rand_prob Probabilidad de descenso de velocidad.
    ///@param new_car_prob Probabilidad de que aparezca un nuevo auto en la posición 0 del AC en la siguiente iteración.
    ///@param new_car_speed Velocidad de nuevo auto cuando ingresa a la pista.
    OpenCAML(const unsigned size, const unsigned int lanes, const double density, const int vmax,
             const double rand_prob, const int init_vel, const double new_car_prob, const int new_car_speed);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param new_car_speed Velocidad de nuevo auto cuando ingresa a la pista.
    OpenCAML(const std::vector<CAElement> &ca, const std::vector<bool> &rand_values, const int vmax,
             const int new_car_speed);

    ///@brief Devuelve referencia a  elemento de valores del autómata celular considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    ///@param lane Carril objetivo.
    int &At(const CaPosition i, const CaLane lane);
    int &AtTemp(const CaPosition i, const CaLane lane);
    int &AtFlowTemp(const CaPosition i, const CaLane lane);

    void Step();    ///< Aplica reglas de evolución temporal del AC.
};


/****************************
*                           *
*      Manejador de CA      *
*                           *
****************************/

/**
* @class CaHandler
* @brief Manejador de CA. Provee interfaz para manejar CA de forma genérica, sin importar el tipo.
*/

class CaHandler
{
    CellularAutomata* cellularautomata;
    CircularCA* circularca;
    OpenCA* openca;
    AutonomousCA* smartca;
    StreetStopCA* streetstopca;
    SemaphoreCA* semaphoreca;
    SimpleJunctionCA* simplejunctionca;
    CellularAutomataML* cellularautomataml;
    CircularCAML* circularcaml;
    OpenCAML* opencaml;

    bool multilane;

public:
    CaHandler();
    CaHandler(CA_TYPE ca, const unsigned size, const double density, const int vmax,
        const double rand_prob, const int init_vel, Args args, const int custom_random_seed = -1);
    CaHandler(CA_TYPE ca, const unsigned size, const unsigned lanes, const double density, const int vmax,
        const double rand_prob, const int init_vel, Args args, const int custom_random_seed = -1);
    ~CaHandler();

    void CreateCa(CA_TYPE ca, const unsigned size, const double density, const int vmax,
        const double rand_prob, const int init_vel, Args args, const int custom_random_seed = -1);
    void CreateCa(CA_TYPE ca, const unsigned size, const unsigned lanes, const double density, const int vmax,
        const double rand_prob, const int init_vel, Args args, const int custom_random_seed = -1);
    void DeleteCa();
    int Status() const;

    void Evolve(const unsigned iter);
    int NextCarDist(const CaPosition pos, const CaLane lane) const;
    bool Randomization(const double prob = -1.0);
    int &At(const CaPosition i, const CaLane lane);
    int GetAt(const CaPosition i, const CaLane lane) const;
    void Connect(CellularAutomata* connect, CaPosition connect_pos);
    void Connect(CellularAutomataML* connect, CaPosition connect_pos);
    int DrawHistory(std::string path = "", std::string out_file_name = "") const;
    int DrawFlowHistory(std::string path = "", std::string out_file_name = "") const;
    void Print() const;
    unsigned GetSize() const;
    unsigned GetHistorySize() const;
    unsigned GetLanes() const;
    unsigned CountCars() const;
    bool IsFluxHalted() const;
    void PrintHistory() const;
    void Step();
    void Move();
    std::vector<double> CalculateOcupancy() const;
    std::vector<double> CalculateFlow() const;
    double CalculateMeanFlow() const;
};

#endif
