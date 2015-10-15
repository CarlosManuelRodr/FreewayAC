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
#include <exception>
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
    CIRCULAR_CA, CIRCULAR_MULTILANE_CA, OPEN_CA, OPEN_MULTILANE_CA, AUTONOMOUS_CA, AUTONOMOUS_NORAND_CA, AUTONOMOUS_INSTONLY_CA , STOP_CA,
    SEMAPHORE_CA, SIMPLE_JUNCTION_CA
};

using CaSize = unsigned;
using CaLane = unsigned;
using CaPosition = int;
using CaVelocity = int;
using CaFlow = char;

const CaVelocity CA_EMPTY = -1;
const CaPosition CA_NULL_POS = -1;
const CaFlow NO_FLOW = 0;
const CaFlow IS_FLOW = 1;

/**
 * @class CellularAutomata
 * @brief Clase base para autómata celular.
 * Esta clase implementa los métodos básicos que todos los autómatas celulares de tráfico poseen.
 * Las condiciones de frontera del autómata se reflejan en los métodos At que se necesitan
 * definir en las clases hijas.
 */
class CellularAutomata
{
protected:
    bool m_test;                 ///< Modo de prueba.
    double m_rand_prob;          ///< Valor de la probabilidad de descenso de velocidad.
    CaVelocity m_vmax;           ///< Valor máximo de la velocidad.
    CaVelocity m_init_vel;       ///< Velocidad inicial de los autos.
    CaSize m_size;               ///< Tamaño del autómata celular
    std::vector<CaVelocity> m_ca;       ///< Automata celular. -1 para casillas sin auto, y valores >= 0 indican velocidad del auto en esa casilla.
    std::vector<CaVelocity> m_ca_temp;
    std::vector<CaFlow> m_ca_flow_temp;                         ///< Variable temporal para operaciones con AC.
    std::vector< std::vector<CaVelocity> > m_ca_history;
    std::vector< std::vector<CaFlow> > m_ca_flow_history;       ///< Lista con valores históricos de AC.
    std::vector<bool> m_rand_values;                            ///< Lista con valores aleatorios para usar en modo de prueba.

    // Conexión de carriles.
    std::vector<CellularAutomata*> m_parents;
    std::vector<CellularAutomata*> m_connect;  ///< Puntero al AC al cual se va a conectar.
    std::vector<CaPosition> m_connect_from, m_connect_to;     ///< Posición del AC donde se realiza la conexión.
    std::vector<double> m_weights;

public:
    ///@brief Constructor.
    ///@param size Tamaño del AC.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param rand_prob Probabilidad de descenso de velocidad.
    CellularAutomata(const CaSize size, const double density, const CaVelocity vmax, const double rand_prob, const CaVelocity init_vel);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param vmax Velocidad máxima de los autos.
    CellularAutomata(const std::vector<int> &ca, const std::vector<bool> &rand_values, const CaVelocity vmax);

    virtual ~CellularAutomata();

    ///@brief Evoluciona (itera) el AC.
    ///@param iter Número de iteraciones.
    virtual void Evolve(const unsigned iter) noexcept;

    ///@brief Devuelve la distancia al auto más próximo desde la posición pos.
    ///@param pos Posición desde dónde iniciar la búsqueda.
    CaSize NextCarDist(const CaPosition pos) const noexcept;

    ///@brief Devuelve valores verdaderos con probabilidad prob. Si se usa en prueba usa valores de lista.
    ///@param prob Probabilidad de obtener valor verdadero. Por defecto se utiliza m_rand_prob.
    bool Randomization(const double prob = -1.0) noexcept;

    ///@brief Devuelve referencia a elemento del AC considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    virtual CaVelocity &At(const CaPosition i) noexcept = 0;
    virtual CaVelocity &AtTemp(const CaPosition i) noexcept = 0;
    virtual CaFlow &AtFlowTemp(const CaPosition i) noexcept = 0;
    virtual CaVelocity GetAt(const CaPosition i) const noexcept = 0;

    ///@brief Devuelve referencia a elemento del AC en conexión.
    ///@param i Posición dentro del AC.
    CaVelocity &AtConnected(const CaPosition i, const unsigned connect_target) noexcept;

    ///@brief Conecta AC con otro. El flujo de autos ocurre desde el que realiza la conexión al objetivo.
    ///@param connect Puntero a AC objetivo.
    ///@param from Posición del AC padre de donde se realiza la conexión.
    ///@param to Posición del AC objetivo donde se realiza la conexión.
    ///@param weight Probabilidad de que el vehiculo entre a la conexion.
    void Connect(CellularAutomata* connect, const CaPosition from, const CaPosition to, const double weight);
    void SetParent(CellularAutomata* parent);

    ///@brief Dibuja mapa histórico del AC en formato BMP.
    ///@param path Ruta del archivo.
    ///@param out_file_name Nombre del archivo de salida.
    virtual void DrawHistory(std::string path = "", std::string out_file_name = "") const;

    ///@brief Dibuja mapa histórico del flujo de AC en formato BMP.
    ///@param path Ruta del archivo.
    ///@param out_file_name Nombre del archivo de salida.
    virtual void DrawFlowHistory(std::string path = "", std::string out_file_name = "") const;
    
    virtual std::vector<double> CalculateOcupancy() const noexcept;
    virtual std::vector<double> CalculateFlow() const noexcept;
    virtual double CalculateMeanFlow() const noexcept;

    void Print() const noexcept;                   ///< Escribe línea de autómata celular en la terminal.
    CaSize GetSize() const noexcept;             ///< Devuelve tamaño del AC.
    CaSize GetHistorySize() const noexcept;      ///< Devuelve tamaño de la lista histórica de evolución del AC.
    unsigned CountCars() const noexcept;           ///< Cuenta la cantidad de autos en AC.
    void PrintHistory() const noexcept;            ///< Escribe los valores históricos del AC en la terminal.
    virtual void Step() noexcept;            ///< Aplica reglas de evolución temporal del AC.
    virtual void Move() noexcept;            ///< Mueve los autos según las condiciones de frontera especificadas en clase hija.
    void AssignChanges() noexcept;           ///< Asigna cambios de los arrays teporales al array m_ca e historico.
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
    CircularCA(const CaSize size, const double density, const CaVelocity vmax, const double rand_prob, const CaVelocity init_vel);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    CircularCA(const std::vector<int> &ca, const std::vector<bool> &rand_values, const CaVelocity vmax);

    ///@brief Devuelve elemento de valores del autómata celular considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    CaVelocity &At(const CaPosition i) noexcept;
    CaVelocity &AtTemp(const CaPosition i) noexcept;
    CaFlow &AtFlowTemp(const CaPosition i) noexcept;
    CaVelocity GetAt(const CaPosition i) const noexcept;

    ///@brief Evoluciona (itera) el AC. Verifica si se conserva la cantidad de autos.
    ///@param iter Número de iteraciones.
    void Evolve(const unsigned iter) noexcept;
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
    CaVelocity m_ca_empty;         ///< Se usa para devolver referencia de lugar vacío.
    CaFlow m_ca_flow_empty;
    double m_new_car_prob;         ///< Probabilidad de que aparezca un nuevo auto en la posición 0 del AC en la siguiente iteración.
    CaVelocity m_new_car_speed;    ///< Velocidad de nuevo auto cuando ingresa a la pista.
public:
    ///@brief Constructor.
    ///@param size Tamaño del AC.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param rand_prob Probabilidad de descenso de velocidad.
    ///@param new_car_prob Probabilidad de que aparezca un nuevo auto en la posición 0 del AC en la siguiente iteración.
    ///@param new_car_speed Velocidad de nuevo auto cuando ingresa a la pista.
    OpenCA(const CaSize size, const double density, const CaVelocity vmax, const double rand_prob, const CaVelocity init_vel,
           const double new_car_prob, const CaVelocity new_car_speed);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param new_car_speed Velocidad de nuevo auto cuando ingresa a la pista.
    OpenCA(const std::vector<int> &ca, const std::vector<bool> &rand_values, const CaVelocity vmax, const CaVelocity new_car_speed);

    ///@brief Devuelve elemento de valores del autómata celular considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    CaVelocity &At(const CaPosition i) noexcept;
    CaVelocity &AtTemp(const CaPosition i) noexcept;
    CaFlow &AtFlowTemp(const CaPosition i) noexcept;
    CaVelocity GetAt(const CaPosition i) const noexcept;

    void Step() noexcept;    ///< Aplica reglas de evolución temporal del AC.
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
    AutonomousCA(const CaSize size, const double density, const CaVelocity vmax, const double rand_prob,
                 const CaVelocity init_vel, const double aut_density);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param aut_cars Lista con posiciones de autos autónomos.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    AutonomousCA(const std::vector<int> &ca, std::vector<int> &aut_cars, const std::vector<bool> &rand_values,
                 const CaVelocity vmax);

    void Move() noexcept;    ///< Mueve los autos con condiciones de frontera periódicas.
    virtual void Step() noexcept;    ///< Aplica reglas de evolución temporal del AC para autos normales e inteligentes.
};

/**
* @class AutonomousNoRandCA
* @brief AC con autos autónomos que sólo tienen la propiedad de no aleatorización. Frontera periódica.
*/
class AutonomousNoRandCA : public AutonomousCA
{
public:
    AutonomousNoRandCA(const CaSize size, const double density, const CaVelocity vmax, const double rand_prob,
        const CaVelocity init_vel, const double aut_density);
    AutonomousNoRandCA(const std::vector<int> &ca, std::vector<int> &aut_cars, const std::vector<bool> &rand_values,
        const CaVelocity vmax);

    void Step() noexcept;
};

/**
* @class AutonomousInstanteneousOnlyCA
* @brief AC con autos autónomos que sólo tienen la propiedad de no aleatorización. Frontera periódica.
*/
class AutonomousInstanteneousOnlyCA : public AutonomousCA
{
public:
    AutonomousInstanteneousOnlyCA(const CaSize size, const double density, const CaVelocity vmax, const double rand_prob,
        const CaVelocity init_vel, const double aut_density);
    AutonomousInstanteneousOnlyCA(const std::vector<int> &ca, std::vector<int> &aut_cars, const std::vector<bool> &rand_values,
        const CaVelocity vmax);

    void Step() noexcept;
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
    StreetStopCA(const CaSize size, const double density, const CaVelocity vmax, const double rand_prob,
                 const CaVelocity init_vel, const double stop_density);

    ///@brief Devuelve la distancia al tope más próximo desde la posición pos.
    ///@param pos Posición desde dónde iniciar la búsqueda.
    CaSize NextStopDist(const CaPosition pos) const noexcept;

    ///@brief Dibuja mapa histórico del AC en formato BMP.
    ///@param path Ruta del archivo.
    ///@param out_file_name Nombre del archivo de salida.
    void DrawHistory(std::string path = "", std::string out_file_name = "") const;

    void Step() noexcept;        ///< Aplica reglas de evolución temporal del AC con tope.
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
    SemaphoreCA(const CaSize size, const double density, const CaVelocity vmax, const double rand_prob,
                const CaVelocity init_vel, const double semaphore_density, const bool random_semaphores = false);

    ///@brief Devuelve la distancia al semáforo más próximo desde la posición pos.
    ///@param pos Posición desde dónde iniciar la búsqueda.
    CaSize NextSemaphoreDist(const CaPosition pos) const noexcept;

    ///@brief Dibuja mapa histórico del AC en formato BMP.
    ///@param path Ruta del archivo.
    ///@param out_file_name Nombre del archivo de salida.
    void DrawHistory(std::string path = "", std::string out_file_name = "") const;

    void Step() noexcept;        ///< Aplica reglas de evolución temporal del AC con tope.
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
    OpenCA *m_target;
    int m_target_lane;
public:
    ///@brief Constructor.
    ///@param size Tamaño del AC.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param rand_prob Probabilidad de descenso de velocidad.
    ///@param return_lane Carril al que apunta la función At.
    SimpleJunctionCA(const CaSize size, const double density, const CaVelocity vmax, const double rand_prob,
                     const CaVelocity init_vel, const double new_car_prob, const CaVelocity new_car_speed, const int target_lane = 0);

    ~SimpleJunctionCA();

    std::vector<double> CalculateOcupancy() const noexcept;
    std::vector<double> CalculateFlow() const noexcept;
    double CalculateMeanFlow() const noexcept;
};


////////////////////////////////////
//                                //
//      Autómatas celulares       //
//      de varios carriles        //
//                                //
////////////////////////////////////

class CaElementVel : public std::vector<CaPosition>
{
public:
    CaElementVel(const CaLane lanes, const CaPosition def_val = CA_EMPTY);
};

class CaElementFlow : public std::vector<CaFlow>
{
public:
    CaElementFlow(const CaLane lanes, const CaFlow def_val = NO_FLOW);
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
    double m_rand_prob;          ///< Valor de la probabilidad de descenso de velocidad.
    CaVelocity m_vmax;           ///< Valor máximo de la velocidad.
    CaLane m_lanes;              ///< Número de carriles.
    CaVelocity m_init_vel;       ///< Velocidad inicial de los vehículos.
    CaSize m_size;               ///< Tamaño del autómata celular
    std::vector<CaElementVel> m_ca;    ///< Automata celular. -1 para casillas sin auto, y valores >= 0 indican velocidad del auto en esa casilla.
    std::vector<CaElementVel> m_ca_temp;                         ///< Variable temporal para operaciones con AC.
    std::vector<CaElementFlow> m_ca_flow_temp;
    std::vector< std::vector<CaElementVel> > m_ca_history;       ///< Lista con valores históricos de AC.
    std::vector< std::vector<CaElementFlow> > m_ca_flow_history;
    std::vector<bool> m_rand_values;                                    ///< Lista con valores aleatorios para usar en modo de prueba.

    // Conexión de carriles.
    std::vector<CellularAutomataML*> m_parents;
    std::vector<CellularAutomataML*> m_connect;  ///< Puntero al AC al cual se va a conectar.
    std::vector<CaPosition> m_connect_from, m_connect_to;     ///< Posición del AC donde se realiza la conexión.
    std::vector<double> m_weights;

public:
    ///@brief Constructor.
    ///@param size Tamaño del AC.
    ///@param lanes Número de carriles.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param rand_prob Probabilidad de descenso de velocidad.
    CellularAutomataML(const CaSize size, const CaLane lanes, const double density,
                       const CaVelocity vmax, const double rand_prob, const CaVelocity init_vel);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    CellularAutomataML(const std::vector<CaElementVel> &ca, const std::vector<bool> &rand_values,
                       const CaVelocity vmax);

    virtual ~CellularAutomataML();

    ///@brief Evoluciona (itera) el AC.
    ///@param iter Número de iteraciones.
    void Evolve(const unsigned iter) noexcept;

    ///@brief Devuelve referencia a  elemento de valores del autómata celular considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    ///@param lane Carril objetivo.
    virtual CaVelocity &At(const CaPosition i, const CaLane lane) noexcept = 0;
    virtual CaVelocity &AtTemp(const CaPosition i, const CaLane lane) noexcept = 0;
    virtual CaFlow &AtFlowTemp(const CaPosition i, const CaLane lane) noexcept = 0;

    ///@brief Similar a AC pero sólo devuelve el valor y en AC de uniones puede apuntar a un carril en específico.
    ///@param i Posición dentro del AC.
    ///@param ca Tipo de AC.
    virtual CaVelocity GetAt(const CaPosition i, const CaLane lane) const noexcept = 0;

    ///@brief Devuelve referencia a elemento del AC en conexión.
    ///@param i Posición dentro del AC.
    CaVelocity &AtConnected(const CaPosition i, const CaLane lane, const unsigned connect_target) noexcept;

    ///@brief Conecta AC con otro. El flujo de autos ocurre desde el que realiza la conexión al objetivo.
    ///@param connect Puntero a AC objetivo.
    ///@param from Posición del AC padre de donde se realiza la conexión.
    ///@param to Posición del AC objetivo donde se realiza la conexión.
    ///@param weight Probabilidad de que el vehiculo entre a la conexion.
    void Connect(CellularAutomataML* connect, const CaPosition from, const CaPosition to, const double weight);
    void SetParent(CellularAutomataML* parent);

    ///@brief Devuelve la distancia al auto más próximo desde la posición pos.
    ///@param pos Posición desde dónde iniciar la búsqueda.
    virtual CaSize NextCarDist(const CaPosition pos, const CaLane lane) const noexcept;

    ///@brief Dibuja mapa histórico del AC en formato BMP.
    ///@param path Ruta del archivo.
    ///@param out_file_name Nombre del archivo de salida.
    void DrawHistory(std::string path = "", std::string out_file_name = "") const;

    ///@brief Dibuja mapa histórico del flujo de AC en formato BMP.
    ///@param path Ruta del archivo.
    ///@param out_file_name Nombre del archivo de salida.
    void DrawFlowHistory(std::string path = "", std::string out_file_name = "") const;

    ///@brief Devuelve valores verdaderos con probabilidad prob. Si se usa en prueba usa valores de lista.
    ///@param prob Probabilidad de obtener valor verdadero. Por defecto se utiliza m_rand_prob.
    bool Randomization(const double prob = -1.0) noexcept;

    std::vector<double> CalculateOcupancy() const noexcept;
    std::vector<double> CalculateFlow() const noexcept;
    double CalculateMeanFlow() const noexcept;

    void Print() const noexcept;               ///< Escribe línea de autómata celular en la terminal.
    CaSize GetSize() const noexcept;           ///< Devuelve tamaño del AC.
    CaSize GetHistorySize() const noexcept;    ///< Devuelve tamaño de la lista histórica de evolución del AC.
    CaLane GetLanes() const noexcept;        ///< Devuelve el número de carriles.
    unsigned CountCars() const noexcept;       ///< Cuenta la cantidad de autos en AC.
    void PrintHistory() const noexcept;        ///< Escribe los valores históricos del AC en la terminal.
    virtual void Step() noexcept;        ///< Aplica reglas de evolución temporal del AC.
    virtual void ChangeLanes() noexcept; ///< Aplica las reglas de cambio carril.
    virtual void Move() noexcept;        ///< Mueve los autos según las condiciones de frontera especificadas en clase hija.
    void AssignChanges() noexcept;           ///< Asigna cambios de los arrays teporales al array m_ca e historico.
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
    CircularCAML(const CaSize size, const CaLane lanes, const double density,
                 const CaVelocity vmax, const double rand_prob, const CaVelocity init_vel);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    CircularCAML(const std::vector<CaElementVel> &ca, const std::vector<bool> &rand_values, const CaVelocity vmax);

    ///@brief Devuelve referencia a  elemento de valores del autómata celular considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    ///@param lane Carril objetivo.
    CaVelocity &At(const CaPosition i, const CaLane lane) noexcept;
    CaVelocity &AtTemp(const CaPosition i, const CaLane lane) noexcept;
    CaFlow &AtFlowTemp(const CaPosition i, const CaLane lane) noexcept;
    CaVelocity GetAt(const CaPosition i, const CaLane lane) const noexcept;

    ///@brief Evoluciona (itera) el AC. Verifica si se conserva la cantidad de autos.
    ///@param iter Número de iteraciones.
    void Evolve(const unsigned iter) noexcept;
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
    CaVelocity m_ca_empty;  ///< Se usa para devolver referencia de lugar vacío.
    CaFlow m_ca_flow_empty;
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
    OpenCAML(const CaSize size, const CaLane lanes, const double density, const CaVelocity vmax,
             const double rand_prob, const CaVelocity init_vel, const double new_car_prob, const CaVelocity new_car_speed);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param new_car_speed Velocidad de nuevo auto cuando ingresa a la pista.
    OpenCAML(const std::vector<CaElementVel> &ca, const std::vector<bool> &rand_values, const CaVelocity vmax,
             const CaVelocity new_car_speed);

    ///@brief Devuelve referencia a  elemento de valores del autómata celular considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    ///@param lane Carril objetivo.
    CaVelocity &At(const CaPosition i, const CaLane lane) noexcept;
    CaVelocity &AtTemp(const CaPosition i, const CaLane lane) noexcept;
    CaFlow &AtFlowTemp(const CaPosition i, const CaLane lane) noexcept;
    CaVelocity GetAt(const CaPosition i, const CaLane lane) const noexcept;

    void Step() noexcept;    ///< Aplica reglas de evolución temporal del AC.
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
    AutonomousNoRandCA* smartnorandca;
    AutonomousInstanteneousOnlyCA* smartinstonlyca;
    StreetStopCA* streetstopca;
    SemaphoreCA* semaphoreca;
    SimpleJunctionCA* simplejunctionca;
    CellularAutomataML* cellularautomataml;
    CircularCAML* circularcaml;
    OpenCAML* opencaml;

    bool multilane;

public:
    CaHandler();
    CaHandler(CA_TYPE ca, const CaSize size, const double density, const CaVelocity vmax,
        const double rand_prob, const CaVelocity init_vel, Args args, const int custom_random_seed = -1);
    CaHandler(CA_TYPE ca, const CaSize size, const CaLane lanes, const double density, const CaVelocity vmax,
        const double rand_prob, const CaVelocity init_vel, Args args, const int custom_random_seed = -1);
    ~CaHandler();

    void CreateCa(CA_TYPE ca, const CaSize size, const double density, const CaVelocity vmax,
        const double rand_prob, const CaVelocity init_vel, Args args, const int custom_random_seed = -1);
    void CreateCa(CA_TYPE ca, const CaSize size, const CaLane lanes, const double density, const CaVelocity vmax,
        const double rand_prob, const CaVelocity init_vel, Args args, const int custom_random_seed = -1);
    void DeleteCa();

    void Evolve(const unsigned iter) noexcept;
    CaSize NextCarDist(const CaPosition pos, const CaLane lane) const noexcept;
    bool Randomization(const double prob = -1.0) noexcept;
    CaVelocity &At(const CaPosition i, const CaLane lane) noexcept;
    CaVelocity GetAt(const CaPosition i, const CaLane lane) const noexcept;
    void DrawHistory(std::string path = "", std::string out_file_name = "") const;
    void DrawFlowHistory(std::string path = "", std::string out_file_name = "") const;
    void Print() const noexcept;
    CaSize GetSize() const noexcept;
    CaSize GetHistorySize() const noexcept;
    CaLane GetLanes() const noexcept;
    unsigned CountCars() const noexcept;
    void PrintHistory() const noexcept;
    void Step() noexcept;
    void Move() noexcept;
    std::vector<double> CalculateOcupancy() const noexcept;
    std::vector<double> CalculateFlow() const noexcept;
    double CalculateMeanFlow() const noexcept;
};

#endif