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
    CellularAutomata* m_connect; ///< Puntero al AC al cual se va a conectar.
    unsigned m_connect_pos;      ///< Posición del AC objetivo donde se realiza la conexión.

public:
    ///@brief Constructor.
    ///@param size Tamaño del AC.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param rand_prob Probabilidad de descenso de velocidad.
    CellularAutomata(const unsigned &size, const double &density, const int &vmax, const double &rand_prob, const int &init_vel);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    CellularAutomata(const std::vector<int> &ca, const std::vector<bool> &rand_values, const int &vmax);

    virtual ~CellularAutomata();

    ///@brief Evoluciona (itera) el AC.
    ///@param iter Número de iteraciones.
    virtual void Evolve(const unsigned &iter);

    ///@brief Devuelve la distancia al auto más próximo desde la posición pos.
    ///@param pos Posición desde dónde iniciar la búsqueda.
    int NextCarDist(const int &pos);

    ///@brief Devuelve valores verdaderos con probabilidad prob. Si se usa en prueba usa valores de lista.
    ///@param prob Probabilidad de obtener valor verdadero. Por defecto se utiliza m_rand_prob.
    bool Randomization(const double &prob = -1.0);

    ///@brief Devuelve referencia a elemento del AC considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    ///@param ca Tipo de AC.
    int &At(const int &i, const CAS &ca = CA);

    ///@brief Devuelve referencia a  elemento de valores del autómata celular considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    ///@param j Posición temporal del AC.
    ///@param ca Tipo de autómata celular.
    virtual int &At(const unsigned &i, const unsigned &j, const CAS &ca) = 0;

    ///@brief Similar a AC pero sólo devuelve el valor y en AC de uniones puede apuntar a un carril en específico.
    ///@param i Posición dentro del AC.
    ///@param ca Tipo de AC.
    int GetAt(const unsigned &i, const CAS &ca = CA);

    ///@brief Similar a AC pero sólo devuelve el valor y en AC de uniones puede apuntar a un carril en específico.
    ///@param i Posición dentro del AC.
    ///@param j Posición temporal del AC.
    ///@param ca Tipo de autómata celular.
    virtual int GetAt(const unsigned &i, const unsigned &j, const CAS &ca);

    ///@brief Conecta AC con otro. El flujo de autos ocurre desde el que realiza la conexión al objetivo.
    ///@param connect Puntero a AC objetivo.
    ///@param connect_pos Posición del AC objetivo donde se realiza la conexión.
    void Connect(CellularAutomata* connect, unsigned connect_pos);

    ///@brief Dibuja mapa histórico del AC en formato BMP.
    ///@param path Ruta del archivo.
    ///@param out_file_name Nombre del archivo de salida.
    ///@return 0 si se pudo crear archivo, 1 en caso de error.
    virtual int DrawHistory(std::string path = "", std::string out_file_name = "");

    ///@brief Dibuja mapa histórico del flujo de AC en formato BMP.
    ///@param path Ruta del archivo.
    ///@param out_file_name Nombre del archivo de salida.
    ///@return 0 si se pudo crear archivo, 1 en caso de error.
    virtual int DrawFlowHistory(std::string path = "", std::string out_file_name = "");
    
    std::vector<double> CalculateOcupancy();
    std::vector<double> CalculateFlow();
    double CalculateMeanFlow();

    void Print();                   ///< Escribe línea de autómata celular en la terminal.
    unsigned GetSize();             ///< Devuelve tamaño del AC.
    unsigned GetHistorySize();      ///< Devuelve tamaño de la lista histórica de evolución del AC.
    unsigned CountCars();           ///< Cuenta la cantidad de autos en AC.
    bool IsFluxHalted();        ///< Informa si el flujo está estancado.
    void PrintHistory();            ///< Escribe los valores históricos del AC en la terminal.
    virtual void Step();            ///< Aplica reglas de evolución temporal del AC.
    virtual void Move();            ///< Mueve los autos según las condiciones de frontera especificadas en clase hija.
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
    CircularCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob, const int &init_vel);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    CircularCA(const std::vector<int> &ca, const std::vector<bool> &rand_values, const int &vmax);

    ///@brief Devuelve elemento de valores del autómata celular considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    ///@param j Posición temporal del AC.
    ///@param ca Tipo de autómata celular.
    using CellularAutomata::At;
    int &At(const unsigned &i, const unsigned &j, const CAS &ca);

    ///@brief Evoluciona (itera) el AC. Verifica si se conserva la cantidad de autos.
    ///@param iter Número de iteraciones.
    void Evolve(const unsigned &iter);
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
    int m_empty;            ///< Se usa para devolver referencia de lugar vacío.
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
    OpenCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob, const int &init_vel,
           const double &new_car_prob, const int &new_car_speed);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param new_car_speed Velocidad de nuevo auto cuando ingresa a la pista.
    OpenCA(const std::vector<int> &ca, const std::vector<bool> &rand_values, const int &vmax, const int &new_car_speed);

    ///@brief Devuelve elemento de valores del autómata celular considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    ///@param j Posición temporal del AC.
    ///@param ca Tipo de autómata celular.
    using CellularAutomata::At;
    int &At(const unsigned &i, const unsigned &j, const CAS &ca);

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
    AutonomousCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob,
                 const int &init_vel, const double &aut_density);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param aut_cars Lista con posiciones de autos autónomos.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    AutonomousCA(const std::vector<int> &ca, std::vector<int> aut_cars, const std::vector<bool> &rand_values,
                 const int &vmax);

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
    StreetStopCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob,
                 const int &init_vel, const double &stop_density);

    ///@brief Devuelve la distancia al tope más próximo desde la posición pos.
    ///@param pos Posición desde dónde iniciar la búsqueda.
    int NextStopDist(const int &pos);

    ///@brief Dibuja mapa histórico del AC en formato BMP.
    ///@param path Ruta del archivo.
    ///@param out_file_name Nombre del archivo de salida.
    ///@return 0 si se pudo crear archivo, 1 en caso de error.
    int DrawHistory(std::string path = "", std::string out_file_name = "");

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
    std::vector<unsigned> m_semaphore_pos;                ///< Lista con posiciones de los semáforos.
    std::vector<int> m_semaphore_value;                   ///< Valor inicial del tiempo del semáforo.
    std::vector< std::vector<int> > m_semaphore_val_hist; ///< Valores historicos de valores de semaforo.
    int m_semaphore_init;                                  ///< Valor del contador de semáforo con el cual comienza.
    int m_semaphore_open;                                 ///< Valor del contador del semáforo en el cual se abre.
public:
    ///@brief Constructor.
    ///@param size Tamaño del AC.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param rand_prob Probabilidad de descenso de velocidad.
    ///@param semaphore_density Densidad de semaforos respecto a tamaño total del AC.
    ///@param 
    SemaphoreCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob,
                const int &init_vel, const double &semaphore_density, const bool &random_semaphores = false);

    ///@brief Devuelve la distancia al semáforo más próximo desde la posición pos.
    ///@param pos Posición desde dónde iniciar la búsqueda.
    int NextSemaphoreDist(const int &pos);

    ///@brief Dibuja mapa histórico del AC en formato BMP.
    ///@param path Ruta del archivo.
    ///@param out_file_name Nombre del archivo de salida.
    ///@return 0 si se pudo crear archivo, 1 en caso de error.
    int DrawHistory(std::string path = "", std::string out_file_name = "");

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
    SimpleJunctionCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob,
                     const int &init_vel, const double &new_car_prob, const int &new_car_speed, const int &target_lane = 0);

    ~SimpleJunctionCA();

    ///@brief Evoluciona (itera) el AC. Verifica si se conserva la cantidad de autos.
    ///@param iter Número de iteraciones.
    void Evolve(const unsigned &iter);

    ///@brief Devuelve el valor de AC apuntando al carril m_target_lane.
    ///@param i Posición dentro del AC.
    ///@param j Posición temporal del AC.
    ///@param ca Tipo de autómata celular.
    int GetAt(const unsigned &i, const unsigned &j, const CAS &ca);

    ///@brief Dibuja mapa histórico del AC en formato BMP.
    ///@param path Ruta del archivo.
    ///@param out_file_name Nombre del archivo de salida.
    ///@return 0 si se pudo crear archivo, 1 en caso de error.
    int DrawHistory(std::string path = "", std::string out_file_name = "");
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
    unsigned m_connect_pos;      ///< Posición del AC objetivo donde se realiza la conexión.

public:
    ///@brief Constructor.
    ///@param size Tamaño del AC.
    ///@param lanes Número de carriles.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param rand_prob Probabilidad de descenso de velocidad.
    CellularAutomataML(const unsigned &size, const unsigned &lanes, const double &density, 
                       const int &vmax, const double &rand_prob, const int &init_vel);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    CellularAutomataML(const std::vector<CAElement> &ca, const std::vector<bool> &rand_values,
                       const int &vmax);

    virtual ~CellularAutomataML();

    ///@brief Evoluciona (itera) el AC.
    ///@param iter Número de iteraciones.
    void Evolve(const unsigned &iter);

    ///@brief Devuelve referencia a elemento del AC considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    ///@param lane Carril objetivo.
    ///@param ca Tipo de AC.
    int &At(const int &i, const unsigned &lane, const CAS &ca = CA);

    ///@brief Devuelve referencia a  elemento de valores del autómata celular considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    ///@param lane Carril objetivo.
    ///@param j Posición temporal del AC.
    ///@param ca Tipo de autómata celular.
    virtual int &At(const int &i, const unsigned &lane, const unsigned &j, const CAS &ca) = 0;

    ///@brief Similar a AC pero sólo devuelve el valor y en AC de uniones puede apuntar a un carril en específico.
    ///@param i Posición dentro del AC.
    ///@param ca Tipo de AC.
    int GetAt(const unsigned &i, const unsigned &lane, const CAS &ca = CA);

    ///@brief Similar a AC pero sólo devuelve el valor y en AC de uniones puede apuntar a un carril en específico.
    ///@param i Posición dentro del AC.
    ///@param j Posición temporal del AC.
    ///@param ca Tipo de autómata celular.
    virtual int GetAt(const unsigned &i, const unsigned &lane, const unsigned &j, const CAS &ca);

    ///@brief Conecta AC con otro. El flujo de autos ocurre desde el que realiza la conexión al objetivo.
    ///@param connect Puntero a AC objetivo.
    ///@param connect_pos Posición del AC objetivo donde se realiza la conexión.
    void Connect(CellularAutomataML* connect, unsigned connect_pos);

    ///@brief Devuelve la distancia al auto más próximo desde la posición pos.
    ///@param pos Posición desde dónde iniciar la búsqueda.
    virtual int NextCarDist(const int &pos, const unsigned &lane);

    ///@brief Dibuja mapa histórico del AC en formato BMP.
    ///@param path Ruta del archivo.
    ///@param out_file_name Nombre del archivo de salida.
    ///@return 0 si se pudo crear archivo, 1 en caso de error.
    int DrawHistory(std::string path = "", std::string out_file_name = "");

    ///@brief Dibuja mapa histórico del flujo de AC en formato BMP.
    ///@param path Ruta del archivo.
    ///@param out_file_name Nombre del archivo de salida.
    ///@return 0 si se pudo crear archivo, 1 en caso de error.
    int DrawFlowHistory(std::string path = "", std::string out_file_name = "");

    ///@brief Devuelve valores verdaderos con probabilidad prob. Si se usa en prueba usa valores de lista.
    ///@param prob Probabilidad de obtener valor verdadero. Por defecto se utiliza m_rand_prob.
    bool Randomization(const double &prob = -1.0);

    void Print();               ///< Escribe línea de autómata celular en la terminal.
    unsigned GetSize();         ///< Devuelve tamaño del AC.
    unsigned GetHistorySize();  ///< Devuelve tamaño de la lista histórica de evolución del AC.
    unsigned GetLanes();        ///< Devuelve el número de carriles.
    unsigned CountCars();       ///< Cuenta la cantidad de autos en AC.
    virtual void Step();        ///< Aplica reglas de evolución temporal del AC.
    virtual void ChangeLanes(); ///< Aplica las reglas de cambio carril.
    virtual void Move();        ///< Mueve los autos según las condiciones de frontera especificadas en clase hija.
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
    CircularCAML(const unsigned &size, const unsigned int &lanes, const double &density,
                 const int &vmax, const double &rand_prob, const int &init_vel);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    CircularCAML(const std::vector<CAElement> &ca, const std::vector<bool> &rand_values, const int &vmax);

    ///@brief Devuelve referencia a  elemento de valores del autómata celular considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    ///@param lane Carril objetivo.
    ///@param j Posición temporal del AC.
    ///@param ca Tipo de autómata celular.
    using CellularAutomataML::At;
    int &At(const int &i, const unsigned &lane, const unsigned &j, const CAS &ca);

    ///@brief Evoluciona (itera) el AC. Verifica si se conserva la cantidad de autos.
    ///@param iter Número de iteraciones.
    void Evolve(const unsigned &iter);
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
    int m_empty;            ///< Se usa para devolver referencia de lugar vacío.
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
    OpenCAML(const unsigned &size, const unsigned int &lanes, const double &density, const int &vmax,
             const double &rand_prob, const int &init_vel, const double &new_car_prob, const int &new_car_speed);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param new_car_speed Velocidad de nuevo auto cuando ingresa a la pista.
    OpenCAML(const std::vector<CAElement> &ca, const std::vector<bool> &rand_values, const int &vmax,
             const int &new_car_speed);

    ///@brief Devuelve elemento de valores del autómata celular considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    ///@param j Posición temporal del AC.
    ///@param ca Tipo de autómata celular.
    using CellularAutomataML::At;
    int &At(const int &i, const unsigned &lane, const unsigned &j, const CAS &ca);

    void Step();    ///< Aplica reglas de evolución temporal del AC.
};

/********************************
*                               *
*  Manejador de CA Multicarril  *
*                               *
********************************/

/**
* @class CaHandlerML
* @brief Manejador de CA multicarril. Provee interfaz para manejar CA de forma genérica, sin importar el tipo.
*/
class CaHandlerML
{
    CellularAutomataML* cellularautomataml;
    CircularCAML* circularcaml;
    OpenCAML* opencaml;

public:
    CaHandlerML();
    CaHandlerML(CA_TYPE ca, const unsigned &size, const unsigned &lanes, const double &density, const int &vmax,
                const double &rand_prob, const int &init_vel, Args args, const int &custom_random_seed = -1);
    ~CaHandlerML();
    void CreateCa(CA_TYPE ca, const unsigned &size, const unsigned &lanes, const double &density, const int &vmax,
                  const double &rand_prob, const int &init_vel, Args args, const int &custom_random_seed = -1);
    void DeleteCa();
    int Status();

    void Evolve(const unsigned &iter);
    int &At(const int &i, const unsigned &lane, const CAS &ca = CA);
    int &At(const int &i, const unsigned &lane, const unsigned &j, const CAS &ca);
    int GetAt(const unsigned &i, const unsigned &lane, const CAS &ca = CA);
    int GetAt(const unsigned &i, const unsigned &lane, const unsigned &j, const CAS &ca);
    void Connect(CellularAutomataML* connect, unsigned connect_pos);
    int NextCarDist(const int &pos, const unsigned &lane);
    int DrawHistory(std::string path = "", std::string out_file_name = "");
    int DrawFlowHistory(std::string path = "", std::string out_file_name = "");
    bool Randomization(const double &prob = -1.0);
    void Print();
    unsigned GetSize();
    unsigned GetHistorySize();
    unsigned GetLanes();
    unsigned CountCars();
    void Step();
    void ChangeLanes();
    void Move();
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
	CaHandler(CA_TYPE ca, const unsigned &size, const double &density, const int &vmax,
		const double &rand_prob, const int &init_vel, Args args, const int &custom_random_seed = -1);
	CaHandler(CA_TYPE ca, const unsigned &size, const unsigned &lanes, const double &density, const int &vmax,
		const double &rand_prob, const int &init_vel, Args args, const int &custom_random_seed = -1);
	~CaHandler();

	void CreateCa(CA_TYPE ca, const unsigned &size, const double &density, const int &vmax,
		const double &rand_prob, const int &init_vel, Args args, const int &custom_random_seed = -1);
	void CreateCa(CA_TYPE ca, const unsigned &size, const unsigned &lanes, const double &density, const int &vmax,
		const double &rand_prob, const int &init_vel, Args args, const int &custom_random_seed = -1);
	void DeleteCa();
	int Status();

	void Evolve(const unsigned &iter);
	int NextCarDist(const int &pos);
	bool Randomization(const double &prob = -1.0);
	int &At(const int &i, const unsigned &lane, const CAS &ca = CA);
	int &At(const int &i, const unsigned &lane, const unsigned &j, const CAS &ca);
	int GetAt(const unsigned &i, const unsigned &lane, const CAS &ca = CA);
	int GetAt(const unsigned &i, const unsigned &lane, const unsigned &j, const CAS &ca);
	void Connect(CellularAutomata* connect, unsigned connect_pos);
	int DrawHistory(std::string path = "", std::string out_file_name = "");
	int DrawFlowHistory(std::string path = "", std::string out_file_name = "");
	void Print();
	unsigned GetSize();
	unsigned GetHistorySize();
	unsigned GetLanes();
	unsigned CountCars();
	bool IsFluxHalted();
	void PrintHistory();
	void Step();
	void Move();
	std::vector<double> CalculateOcupancy();
	std::vector<double> CalculateFlow();
	double CalculateMeanFlow();
};

#endif
