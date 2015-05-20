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

////////////////////////////////////
//                                //
//        Autómatas celulares     //
//        de carril único.        //
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
    CIRCULAR_CA, OPEN_CA, SMART_CA, STOP_CA
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
    void Evolve(const unsigned &iter);

    ///@brief Devuelve la distancia al auto más próximo desde la posición pos.
    ///@param pos Posición desde dónde iniciar la búsqueda.
    int NextCarDist(const int &pos);

	///@brief Devuelve valores verdaderos con probabilidad prob. Si se usa en prueba usa valores de lista.
	///@param prob Probabilidad de obtener valor verdadero. Por defecto se utiliza m_rand_prob.
	bool Randomization(const double &prob = -1.0);

    ///@brief Devuelve elemento del AC considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    ///@param ca Tipo de AC.
    int &At(const unsigned &i, const CAS &ca = CA);

    ///@brief Devuelve elemento de valores del autómata celular considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    ///@param j Posición temporal del AC.
    ///@param ca Tipo de autómata celular.
    virtual int &At(const unsigned &i, const unsigned &j, const CAS &ca) = 0;

    void Print();                   ///< Escribe línea de autómata celular en la terminal.
    unsigned GetSize();             ///< Devuelve tamaño del AC.
    unsigned GetHistorySize();      ///< Devuelve tamaño de la lista histórica de evolución del AC.
    unsigned CountCars();           ///< Cuenta la cantidad de autos en AC.
	void PrintHistory();			///< Escribe los valores históricos del AC en la terminal.
    virtual void DrawHistory();     ///< Dibuja mapa histórico del AC en formato BMP.
    virtual void DrawFlowHistory(); ///< Dibuja mapa histórico del flujo de AC en formato BMP.
    virtual void Step();            ///< Aplica reglas de evolución temporal del AC.
    virtual void Move() = 0;        ///< Mueve los autos según las condiciones de frontera especificadas en clase hija.
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

    virtual void Move();    ///< Mueve los autos con condiciones de frontera periódicas.
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
    int m_empty;            ///< Se usa para devolver referencia de lugar vacío.
    double m_new_car_prob;  ///< Probabilidad de que aparezca un nuevo auto en la posición 0 del AC en la siguiente iteración.
public:
    ///@brief Constructor.
    ///@param size Tamaño del AC.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param rand_prob Probabilidad de descenso de velocidad.
    ///@param new_car_prob Probabilidad de que aparezca un nuevo auto en la posición 0 del AC en la siguiente iteración.
    OpenCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob, const double &new_car_prob);

	///@brief Constructor.
    ///@param ca Lista con valores de AC.
    ///@param rand_values Valores aleatorios en cada paso.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    OpenCA(const std::vector<int> &ca, const std::vector<bool> &rand_values, const int &vmax);

    ///@brief Devuelve elemento de valores del autómata celular considerando las condiciones de frontera.
    ///@param i Posición dentro del AC.
    ///@param j Posición temporal del AC.
    ///@param ca Tipo de autómata celular.
	using CellularAutomata::At;
    int &At(const unsigned &i, const unsigned &j, const CAS &ca);

    void Move();    ///< Mueve los autos con condiciones de frontera abiertas.
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
    std::vector<int> m_smart_cars;  ///< Lista con posiciones de autos inteligentes.
public:
    ///@brief Constructor.
    ///@param size Tamaño del AC.
    ///@param density Densidad de autos.
    ///@param vmax Velocidad máxima de los autos.
    ///@param rand_prob Probabilidad de descenso de velocidad.
    ///@param smart_density Densidad de autos inteligentes respecto a número total de autos.
    SmartCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob, const double &smart_density);

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

    void Step();        ///< Aplica reglas de evolución temporal del AC con tope.
    void DrawHistory(); ///< Dibuja mapa histórico del AC en formato BMP marcando los topes de verde.
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
* @return Puntero de clase base que apunta hacia el AC.
*/
CellularAutomata* create_ca(CA_TYPE ca, const unsigned &size, const double &density, const int &vmax, const double &rand_prob, const double &extra = 0.0);

/**
* @brief Borra cualquier AC que haya sido creado anteriormente.
*/
void delete_ca();



////////////////////////////////////
//                                //
//      Autómatas celulares       //
//      de varios carriles.       //
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

class CellularAutomataML
{
protected:
    int m_vmax;
    unsigned m_lanes;
    double m_rand_prob;
    unsigned m_size;
    std::vector<CAElement> m_ca, m_ca_temp, m_ca_flow_temp;    // Automata celular. -1 para casillas sin auto, y valores >= 0 indican velocidad del auto en esa casilla.
    std::vector< std::vector<CAElement> > m_ca_history, m_ca_flow_history;
    MTRand m_drand;            // Generador de aleatorios (flotantes) entre 0 y 1.
    MTRand_int32 m_irand;      // Generador de enteros aleatorios.

public:
    CellularAutomataML(const unsigned &size, const unsigned &lanes, const double &density, 
                       const int &vmax, const double &rand_prob);
    virtual ~CellularAutomataML();
    void Print();
    void DrawHistory();
    void DrawFlowHistory();
    void Evolve(const unsigned &iter, const bool &print = false);
    unsigned GetSize();
    unsigned GetHistorySize();
    int &At(const int &i, const unsigned &lane, const CAS &ca = CA);
    virtual void Step();
    virtual int &At(const int &i, const unsigned &lane, const unsigned &j, const CAS &ca) = 0;
    virtual int NextCarDist(const int &pos, const unsigned &lane) = 0;
    virtual void Move() = 0;
};

/****************************
*                           *
*      AC Circular ML       *
*                           *
****************************/

class CircularCAML : public CellularAutomataML
{
public:
    CircularCAML(const unsigned size, const unsigned int lanes, const double density, 
                 const int vmax, const double rand_prob);
    using CellularAutomataML::At;
    int &At(const int &i, const unsigned &lane, const unsigned &j, const CAS &ca);
    int NextCarDist(const int &pos, const unsigned &lane);
    virtual void Move();
};

#endif