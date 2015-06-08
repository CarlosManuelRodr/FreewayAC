/**
* @file Auxiliar.h
* @brief Funciones auxiliares.
* @author Carlos Manuel Rodríguez Martínez
* @date 8/06/2015
*/

#pragma once
#ifndef _AUX
#define _AUX

#include <vector>
#include <initializer_list>
#include <numeric>
#include <random>

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
* @brief Crea directorio si no existe.
*/
void aux_create_directory(std::string directory_name);

/**
* @brief Obtiene extensión de nombre de archivo.
*/
std::string aux_get_extension(const std::string filepath);

/**
* @brief Reemplaza ocurrencias en cadena de texto.
*/
std::string aux_replace_all(const std::string &in, const std::string &search, const std::string &replace);

/**
* @brief Reemplaza extensión o la añade si no existe.
*/
std::string aux_replace_extension(const std::string &in, const std::string &ext);

/**
* @brief Reporta valor de datos.
* @param report_type Cadena de texto que indica el tipo de dato a reportar.
* @param report_value  Valor a reportar.
*/
template <class N> void aux_report(const std::string &report_type, N report_value)
{
    std::cout << report_type << ": " << report_value << "." << std::endl;
}

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

/**
* @brief Devuelve el promedio de los elementos de un vector.
*/
template <class N> double aux_mean(std::vector<N> v)
{
    return (double)std::accumulate(v.begin(), v.end(), 0.0)/(double)v.size();
}

/*****************************
*                            *
*   Directorios y archivos   *
*                            *
*****************************/

#if defined(__linux__) || defined(__APPLE__)
    const std::string df_separator = "/";
#elif defined(_WIN32)
    const std::string df_separator = "\\";
#endif

bool df_directory_exist(std::string path);

bool df_file_exist(std::string file_name);

std::string df_get_app_folder();

/****************************
*                           *
*    Llamadas al sistema    *
*                           *
****************************/

/**
* @brief Crea directorio.
* @param arg Ruta al directorio a crear.
*/
void s_mkdir(const std::string arg);

/**
* @brief Borra archivo.
* @param arg Ruta al archivo a borrar.
*/
void s_rm(const std::string arg);

/****************************
*                           *
*  Generador de aleatorios  *
*                           *
****************************/

/**
* @enum RandomAlgorithm
* @brief Tipos de generadores de números aleatorios.
*/
enum RandomAlgorithm
{
    LCG, MT19937, RANLUX24, RANLUX48
};

/**
* @class RandomGen
* @brief Generador de números aleatorios.
*/
class RandomGen
{
    static RandomAlgorithm m_ra;
    static std::mt19937 mt;
    static std::ranlux24 rl24;
    static std::ranlux48 rl48;
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
