#pragma once
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

void aux_progress_bar(double progress);
template <class N> bool aux_is_in(std::vector<N> v, const N find_val)
{
    std::vector<N>::iterator it = find(v.begin(), v.end(), find_val);
    if (it != v.end())
        return true;
    else
        return false;
}
template <class N> int aux_find_pos(std::vector<N> v, const N find_val)
{
    std::vector<N>::iterator it = find(v.begin(), v.end(), find_val);
    if (it != v.end())
        return distance(v.begin(), it);
    else
        return -1;
}
template <class N> N aux_string_to_num(const std::string& s)
{
    std::istringstream i(s);
    N x;
    if (!(i >> x))
        return 0;
    return static_cast<N>(x);
}

/****************************
*                           *
*         AC Básico         *
*                           *
****************************/

enum CAS
{
    CA, CA_TEMP, CA_FLOW_TEMP, CA_HISTORY, CA_FLOW_HISTORY
};

enum CA_TYPE
{
    CIRCULAR_CA, OPEN_CA, SMART_CA
};

class CellularAutomata
{
protected:
    int m_vmax;
    double m_rand_prob;
    unsigned m_size;
    std::vector<int> m_ca, m_ca_temp, m_ca_flow_temp;    // Automata celular. -1 para casillas sin auto, y valores >= 0 indican velocidad del auto en esa casilla.
    std::vector< std::vector<int> > m_ca_history, m_ca_flow_history;
    MTRand m_drand;            // Generador de aleatorios (flotantes) entre 0 y 1.
    MTRand_int32 m_irand;      // Generador de enteros aleatorios.

public:
    CellularAutomata(const unsigned &size, const double &density, const int &vmax, const double &rand_prob);
    void Print();
    void DrawHistory();
    void DrawFlowHistory();
    void Evolve(const unsigned &iter);
    unsigned GetSize();
    unsigned GetHistorySize();
    int &At(const unsigned &i, const CAS &ca = CA);
    virtual void Step();
    virtual int &At(const unsigned &i, const unsigned &j, const CAS &ca) = 0;
    virtual int NextCarDist(const int &pos) = 0;
    virtual void Move() = 0;
};

/****************************
*                           *
*        AC Circular        *
*                           *
****************************/

class CircularCA : public CellularAutomata
{
public:
    CircularCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob);
    using CellularAutomata::At;
    int &At(const unsigned &i, const unsigned &j, const CAS &ca);
    int NextCarDist(const int &pos);
    virtual void Move();
};


/****************************
*                           *
*        AC Abierto         *
*                           *
****************************/

class OpenCA : public CellularAutomata
{
    int empty;
    double m_new_car_prob;
public:
    OpenCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob, const double &new_car_prob);
    using CellularAutomata::At;
    int &At(const unsigned &i, const unsigned &j, const CAS &ca);
    int NextCarDist(const int &pos);
    void Move();
};


/****************************
*                           *
*  AC Coches inteligentes   *
*                           *
****************************/

class SmartCA : public CircularCA
{
    std::vector<int> smart_cars;
public:
    SmartCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob, const double &smart_density);
    void Move();
    void Step();
};


/****************************
*                           *
*      Manejador de CA      *
*                           *
****************************/

CellularAutomata* create_ca(CA_TYPE ct, const unsigned &size, const double &density, const int &vmax, const double &rand_prob, const double &extra = 0.0);
void delete_ca();