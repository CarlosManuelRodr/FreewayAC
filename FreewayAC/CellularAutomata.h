/**
* @file CellularAutomata.h
* @brief Clases de autómatas celulares.
* @author Carlos Manuel Rodriguez Martinez
* @date 18/05/2015
*/

#pragma once
#ifndef _CELLULARAUTOMATA
#define _CELLULARAUTOMATA

#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <initializer_list>
#include "BmpWriter.h"
#include "mtrand.h"

/****************************
*                           *
*   Funciones auxiliares    *
*                           *
****************************/

/**
* @brief Muestra barra de progreso.
* @param progress Valor fraccional del progreso.
*/
void aux_progress_bar(double progress);

/**
* @brief Da semilla a generadores de números aleatorios.
*/
void aux_random_seed();

/**
* @brief Devuelve número aleatorio entre 0 y i-1.
* @param i Límite de número aleatorio.
*/
int aux_random(int i);

/**
* @brief Informa si find_val está dentro de v.
* @param v Vector dónde buscar.
* @param find_val Valor a buscar.
*/
template <class N> bool aux_is_in(std::vector<N> &v, const N &find_val)
{
    typename std::vector<N>::iterator it = find(v.begin(), v.end(), find_val);
    if (it != v.end())
        return true;
    else
        return false;
}

/**
* @brief Informa si find_val está dentro de lista.
* @param list Lista dónde buscar.
* @param find_val Valor a buscar.
*/
template <class N> bool aux_is_in(std::initializer_list<N> list, const N &find_val)
{
	std::vector<N> v = list;
	typename std::vector<N>::iterator it = find(v.begin(), v.end(), find_val);
	if (it != v.end())
		return true;
	else
		return false;
}

/**
* @brief Informa la posición dentro de v donde se encuentra find_val.
* @param v Vector dónde buscar.
* @param find_val Valor a buscar.
* @return La posición ó -1 en caso de que no se encuentre.
*/
template <class N> int aux_find_pos(std::vector<N> &v, const N &find_val)
{
    typename std::vector<N>::iterator it = find(v.begin(), v.end(), find_val);
    if (it != v.end())
        return distance(v.begin(), it);
    else
        return -1;
}

/**
* @brief Convierte cadena de texto a número.
*/
template <class N> N aux_string_to_num(const std::string &s)
{
    std::istringstream i(s);
    N x;
    if (!(i >> x))
        return 0;
    return static_cast<N>(x);
}

/****************************
*                           *
*        Argumentos         *
*                           *
****************************/

/**
* @class Args
* @brief Clase para pasar una cantidad de argumentos variable a una función.
*/
class Args
{
	std::vector<double> m_double_args;
	std::vector<int> m_int_args;
	std::vector<bool> m_bool_args;
public:
	Args();

	///@brief Constructor.
	///@param d_args Argumentos del tipo double.
	///@param i_args Argumentos del tipo int.
	///@param b_args Argumentos del tipo bool.
	Args(std::initializer_list<double> d_args, std::initializer_list<int> i_args = {},
		 std::initializer_list<bool> b_args = {});

	///@brief Devuelve elemento double.
	///@param i Indice del elemento a devolver.
	double GetDouble(const unsigned &i = 0);

	///@brief Devuelve elemento int.
	///@param i Indice del elemento a devolver.
	int GetInt(const unsigned &i = 0);

	///@brief Devuelve elemento bool.
	///@param i Indice del elemento a devolver.
	bool GetBool(const unsigned &i = 0);

	///@brief Asigna elemento double.
	///@param i Indice del elemento a devolver.
	void SetDouble(const unsigned &i, double val);

	///@brief Asigna elemento int.
	///@param i Indice del elemento a devolver.
	void SetInt(const unsigned &i, int val);

	///@brief Asigna elemento bool.
	///@param i Indice del elemento a devolver.
	void SetBool(const unsigned &i, bool val);

	///@brief Copia los valores del argumento.
	///@param arg Objetivo a copiar.
	void operator=(const Args &arg);
};

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
    CIRCULAR_CA, CIRCULAR_MULTILANE_CA, OPEN_CA, SMART_CA, STOP_CA, SEMAPHORE_CA, SIMPLE_JUNCTION_CA
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
    unsigned m_size;             ///< Tamaño del autómata celular
    std::vector<int> m_ca;       ///< Automata celular. -1 para casillas sin auto, y valores >= 0 indican velocidad del auto en esa casilla.
    std::vector<int> m_ca_temp, m_ca_flow_temp;                         ///< Variable temporal para operaciones con AC.
    std::vector< std::vector<int> > m_ca_history, m_ca_flow_history;    ///< Lista con valores históricos de AC.
    std::vector<bool> m_rand_values;                                    ///< Lista con valores aleatorios para usar en modo de prueba.
    MTRand m_drand;              ///< Generador de aleatorios (flotantes) entre 0 y 1.
    MTRand_int32 m_irand;        ///< Generador de enteros aleatorios.

	// Conexión de carriles.
	CellularAutomata* m_connect; ///< Puntero al AC al cual se va a conectar.
	unsigned m_connect_pos;      ///< Posición del AC objetivo donde se realiza la conexión.

public:
    ///@brief Constructor.
    ///@param size Tamaño del AC.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param rand_prob Probabilidad de descenso de velocidad.
    CellularAutomata(const unsigned &size, const double &density, const int &vmax, const double &rand_prob);

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
    int &At(const unsigned &i, const CAS &ca = CA);

    ///@brief Devuelve referencia a  elemento de valores del autómata celular considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    ///@param j Posición temporal del AC.
    ///@param ca Tipo de autómata celular.
    virtual int &At(const unsigned &i, const unsigned &j, const CAS &ca) = 0;

	///@brief Similar a AC pero sólo devuelve el valor y en AC multicarril puede apuntar a un carril en específico.
	///@param i Posición dentro del AC.
	///@param ca Tipo de AC.
	int GetAt(const unsigned &i, const CAS &ca = CA);

	///@brief Similar a AC pero sólo devuelve el valor y en AC multicarril puede apuntar a un carril en específico.
	///@param i Posición dentro del AC.
	///@param j Posición temporal del AC.
	///@param ca Tipo de autómata celular.
	virtual int GetAt(const unsigned &i, const unsigned &j, const CAS &ca);

	///@brief Conecta AC con otro. El flujo de autos ocurre desde el que realiza la conexión al objetivo.
	///@param connect Puntero a AC objetivo.
	///@param connect_pos Posición del AC objetivo donde se realiza la conexión.
	void Connect(CellularAutomata* connect, unsigned connect_pos);

	///@brief Dibuja mapa histórico del AC en formato BMP.
	///@param out_file_name Ruta del archivo de salida.
    virtual void DrawHistory(std::string out_file_name = "");

    ///@brief Dibuja mapa histórico del flujo de AC en formato BMP.
    ///@param out_file_name Ruta del archivo de salida.
    virtual void DrawFlowHistory(std::string out_file_name = "");


    void Print();                   ///< Escribe línea de autómata celular en la terminal.
    unsigned GetSize();             ///< Devuelve tamaño del AC.
    unsigned GetHistorySize();      ///< Devuelve tamaño de la lista histórica de evolución del AC.
    unsigned CountCars();           ///< Cuenta la cantidad de autos en AC.
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
    CircularCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob);

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
    OpenCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob, 
		   const double &new_car_prob, const int &new_car_speed);

    ///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
	///@param new_car_speed Velocidad de nuevo auto cuando ingresa a la pista.
	OpenCA(const std::vector<int> &ca, const std::vector<bool> &rand_values, const int &vmax, 
		   const int &new_car_speed);

    ///@brief Devuelve elemento de valores del autómata celular considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    ///@param j Posición temporal del AC.
    ///@param ca Tipo de autómata celular.
    using CellularAutomata::At;
    int &At(const unsigned &i, const unsigned &j, const CAS &ca);

	void Step();	///< Aplica reglas de evolución temporal del AC.
};


/****************************
*                           *
*  AC Coches inteligentes   *
*                           *
****************************/

/**
 * @class SmartCA
 * @brief AC con autos inteligentes y condiciones de frontera periódicas.
 */
class SmartCA : public CircularCA
{
protected:
    std::vector<int> m_smart_cars;  ///< Lista con posiciones de autos inteligentes.
public:
    ///@brief Constructor.
    ///@param size Tamaño del AC.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param rand_prob Probabilidad de descenso de velocidad.
    ///@param smart_density Densidad de autos inteligentes respecto a número total de autos.
    SmartCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob, 
		    const double &smart_density);

	///@brief Constructor.
	///@param ca Lista con valores de AC.
	///@param smart_cars Lista con posiciones de autos inteligentes.
	///@param rand_values Valores aleatorios en cada paso.
	///@param density Densidad de autos.
	///@param vmax Velocidad máxima de los autos.
	SmartCA(const std::vector<int> &ca, std::vector<int> smart_cars, const std::vector<bool> &rand_values, 
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
    StreetStopCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob, const double &stop_density);

    ///@brief Devuelve la distancia al tope más próximo desde la posición pos.
    ///@param pos Posición desde dónde iniciar la búsqueda.
    int NextStopDist(const int &pos);

    ///@brief Dibuja mapa histórico del AC en formato BMP.
	///@param out_file_name Ruta del archivo de salida.
	void DrawHistory(std::string out_file_name = "");

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
                const double &semaphore_density, const bool &random_semaphores = false);

    ///@brief Devuelve la distancia al semáforo más próximo desde la posición pos.
    ///@param pos Posición desde dónde iniciar la búsqueda.
    int NextSemaphoreDist(const int &pos);

    ///@brief Dibuja mapa histórico del AC en formato BMP.
	///@param out_file_name Ruta del archivo de salida.
	void DrawHistory(std::string out_file_name = "");

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
		             const double &new_car_prob, const int &new_car_speed, const int &target_lane = 0);

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
	///@param out_file_name Ruta del archivo de salida.
	void DrawHistory(std::string out_file_name = "");
};



/****************************
*                           *
*      Manejador de CA      *
*                           *
****************************/

/**
* @brief Crea autómata celular con los parámetros especificados y devuelve puntero genérico.
* @param ca Tipo de AC.
* @param size Tamaño de AC.
* @param density Densidad de autos.
* @param vmax Velocidad máxima.
* @param rand_prob Probabilidad de descenso de velocidad.
* @param args Argumentos extra que requieren algunos AC.
* @return Puntero de clase base que apunta hacia el AC.
*/
CellularAutomata* create_ca(CA_TYPE ca, const unsigned &size, const double &density, const int &vmax, 
                            const double &rand_prob, Args args);

/**
* @brief Borra cualquier AC que haya sido creado anteriormente.
*/
void delete_ca();



////////////////////////////////////
//                                //
//      Autómatas celulares       //
//      de varios carriles        //
//                                //
////////////////////////////////////

class CAElement
{
    std::vector<int> m_lane;
public:
    CAElement(const int lanes, const int def_val = -1);
    int &operator[](const unsigned &iter);
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
	int m_vmax;                  ///< Valor máximo de la velocidad.
    unsigned m_lanes;			 ///< Número de carriles.
	double m_rand_prob;          ///< Valor de la probabilidad de descenso de velocidad.
	unsigned m_size;             ///< Tamaño del autómata celular
	std::vector<CAElement> m_ca;    ///< Automata celular. -1 para casillas sin auto, y valores >= 0 indican velocidad del auto en esa casilla.
	std::vector<CAElement> m_ca_temp, m_ca_flow_temp;                         ///< Variable temporal para operaciones con AC.
	std::vector< std::vector<CAElement> > m_ca_history, m_ca_flow_history;    ///< Lista con valores históricos de AC.
	MTRand m_drand;              ///< Generador de aleatorios (flotantes) entre 0 y 1.
	MTRand_int32 m_irand;        ///< Generador de enteros aleatorios.

public:
	///@brief Constructor.
	///@param size Tamaño del AC.
	///@param lanes Número de carriles.
	///@param density Densidad de autos.
	///@param vmax Velocidad máxima de los autos.
	///@param rand_prob Probabilidad de descenso de velocidad.
    CellularAutomataML(const unsigned &size, const unsigned &lanes, const double &density, 
                       const int &vmax, const double &rand_prob);
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

	///@brief Devuelve la distancia al auto más próximo desde la posición pos.
	///@param pos Posición desde dónde iniciar la búsqueda.
	virtual int NextCarDist(const int &pos, const unsigned &lane);

	///@brief Dibuja mapa histórico del AC en formato BMP.
	///@param out_file_name Ruta del archivo de salida.
    void DrawHistory(std::string out_file_name = "");

    ///@brief Dibuja mapa histórico del flujo de AC en formato BMP.
    ///@param out_file_name Ruta del archivo de salida.
    void DrawFlowHistory(std::string out_file_name = "");

    void Print();			    ///< Escribe línea de autómata celular en la terminal.
    unsigned GetSize();		    ///< Devuelve tamaño del AC.
    unsigned GetHistorySize();  ///< Devuelve tamaño de la lista histórica de evolución del AC.
    unsigned CountCars();           ///< Cuenta la cantidad de autos en AC.
    virtual void Step();        ///< Aplica reglas de evolución temporal del AC.
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
    CircularCAML(const unsigned size, const unsigned int lanes, const double density, 
                 const int vmax, const double rand_prob);

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

/********************************
*                               *
*  Manejador de CA Multicarril  *
*                               *
********************************/

CellularAutomataML* create_multilane_ca(CA_TYPE ca, const unsigned &size, const unsigned &lanes, const double &density,
	                                    const int &vmax, const double &rand_prob, Args args);
void delete_multilane_ca();

#endif
