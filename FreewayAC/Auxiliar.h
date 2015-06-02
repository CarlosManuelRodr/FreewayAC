#pragma once
#ifndef _AUX
#define _AUX

#include <vector>
#include <initializer_list>
#include <numeric>

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
* @brief Muestra barra de progreso.
* @param val Valor en intervalo [0, max].
* @param max Máximo valor.
* @param dt Intervalo de valores.
*/
void aux_progress_bar(double val, double min, double max, double dt);

/**
* @brief Convierte cadena de texto a booleano.
* @param str Texto a convertir. Acepta valores "true" o "false".
*/
bool aux_string_to_bool(std::string str);

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

template <class N> double aux_mean(std::vector<N> v)
{
	return (double)std::accumulate(v.begin(), v.end(), 0.0)/(double)v.size();
}

/****************************
*                           *
*  Generador de aleatorios  *
*                           *
****************************/

enum RandomAlgorithm
{
	LCG, MT19937
};

class RandomGen
{
	static RandomAlgorithm m_ra;
	static std::mt19937 mt;
public:
	static void SetAlgorithm(RandomAlgorithm ra);
	static void Seed(int seed = -1);
	static int GetInt(int i);
	static double GetDouble();
};


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

#endif
