/**
* @file Auxiliar.h
* @brief Funciones auxiliares.
* @author Carlos Manuel Rodríguez Martínez
* @date 8/06/2015
*/

#ifndef _AUX
#define _AUX

#include <vector>
#include <initializer_list>
#include <numeric>
#include <random>
#include <functional>
#include <thread>
#include "ThreadPool.h"

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
* @brief Emite alerta sonora.
*/
void aux_beep();

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


/**
* @brief Lanza threads con argumento en el rango especificado.
* @param f Función a lanzar.
* @param min_val Valor mínimo que se le va a pasar a la función.
* @param max_val Valor máximo que se le va a pasar a la función.
* @param dt Invervalo entre valores que se le pasan a la función.
* @param arg Argumento extra que se le pasa a la función.
*/
template<typename R, typename T, typename Arg>
std::vector<R> aux_parallel_function(std::function<R(T, Arg)> f, T min_val, T max_val, T dt, Arg arg)
{
    ThreadPool pool(std::thread::hardware_concurrency());
    std::vector<std::future<R>> result;
    std::vector<R> result_values;

    for (T val = min_val; val <= max_val; val += dt)
        result.push_back(pool.enqueue(f, val, arg));

    for (unsigned i = 0; i < result.size(); ++i)
    {
        aux_progress_bar(i, 0, result.size(), 1);
        result_values.push_back(result[i].get());
    }
    aux_progress_bar(1, 0, result.size(), 1);
    return result_values;
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

/**
* @brief Borra ruta recursivamente.
* @param path Ruta a borrar.
*/
#if defined(__linux__) || defined(__APPLE__)
int s_rmrf(std::string path);
#endif

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
    std::vector<std::string> m_double_labels, m_int_labels;
    std::vector<std::string> m_bool_labels, m_string_labels;
    std::vector<double> m_double_args;
    std::vector<int> m_int_args;
    std::vector<bool> m_bool_args;
    std::vector<std::string> m_string_args;
public:
    Args();

    ///@brief Constructor.
    ///@param d_args Argumentos del tipo double.
    ///@param i_args Argumentos del tipo int.
    ///@param b_args Argumentos del tipo bool.
    Args(std::initializer_list<double> d_args, std::initializer_list<int> i_args = {},
         std::initializer_list<bool> b_args = {}, std::initializer_list<std::string> s_args = {});

    ///@brief Constructor.
    ///@param d_args Argumentos del tipo double.
    ///@param i_args Argumentos del tipo int.
    ///@param b_args Argumentos del tipo bool.
    Args(std::initializer_list<std::string> d_args_labels, std::initializer_list<double> d_args,
         std::initializer_list<std::string> i_args_labels, std::initializer_list<int> i_args,
         std::initializer_list<std::string> b_args_labels, std::initializer_list<bool> b_args,
         std::initializer_list<std::string> s_args_labels, std::initializer_list<std::string> s_args);

    ///@brief Devuelve elemento double.
    ///@param i Indice del elemento a devolver.
    double GetDouble(const unsigned &i = 0);

    ///@brief Devuelve elemento double.
    ///@param label Etiqueta del elemento a devolver.
    double GetDouble(std::string label);

    ///@brief Devuelve elemento int.
    ///@param i Indice del elemento a devolver.
    int GetInt(const unsigned &i = 0);

    ///@brief Devuelve elemento double.
    ///@param label Etiqueta del elemento a devolver.
    int GetInt(std::string label);

    ///@brief Devuelve elemento bool.
    ///@param i Indice del elemento a devolver.
    bool GetBool(const unsigned &i = 0);

    ///@brief Devuelve elemento bool.
    ///@param label Etiqueta del elemento a devolver.
    bool GetBool(std::string label);

    ///@brief Devuelve elemento string.
    ///@param i Indice del elemento a devolver.
    std::string GetString(const unsigned &i = 0);

    ///@brief Devuelve elemento string.
    ///@param label Etiqueta del elemento a devolver.
    std::string GetString(std::string label);

    ///@brief Asigna elemento double.
    ///@param i Indice del elemento a devolver.
    void SetDouble(const unsigned &i, double val);

    ///@brief Asigna elemento double.
    ///@param label Etiqueta del elemento a devolver.
    void SetDouble(std::string label, double val);

    ///@brief Asigna elemento int.
    ///@param i Indice del elemento a devolver.
    void SetInt(const unsigned &i, int val);

    ///@brief Asigna elemento int.
    ///@param label Etiqueta del elemento a devolver.
    void SetInt(std::string label, int val);

    ///@brief Asigna elemento bool.
    ///@param i Indice del elemento a devolver.
    void SetBool(const unsigned &i, bool val);

    ///@brief Asigna elemento bool.
    ///@param label Etiqueta del elemento a devolver.
    void SetBool(std::string label, bool val);

    ///@brief Asigna elemento string.
    ///@param i Indice del elemento a devolver.
    void SetString(const unsigned &i, std::string val);

    ///@brief Asigna elemento string.
    ///@param label Etiqueta del elemento a devolver.
    void SetString(std::string label, std::string val);

    ///@brief Copia los valores del argumento.
    ///@param arg Objetivo a copiar.
    void operator=(const Args &arg);
};

/****************************
*                           *
*   Contenedor de matriz    *
*                           *
****************************/

/**
* @class Matrix
* @brief Contenedor de matriz
**/
template <class T> class Matrix
{
    T** m_matrix;
    unsigned int m_ncols;
    unsigned int m_nrows;
    int m_status;

    void Deallocate();
public:

    ///@brief Constructor por defecto.
    Matrix();

    ///@brief Constructor copia.
    Matrix(const Matrix<T> &other);

    ///@brief Crea matriz sin especificar elementos.
    Matrix(unsigned int rows, unsigned int cols);

    ///@brief Crea matriz desde arrays.
    Matrix(unsigned int rows, unsigned int cols, T** input);

    ~Matrix();

    // ## Asignación de valores. ##

    ///@brief Crea matriz sin especificar elementos.
    int SetMatrix(unsigned int rows, unsigned int cols);

    ///@brief Crea matriz desde arrays.
    int SetMatrix(unsigned int rows, unsigned int cols, T** input);

    ///@brief Copia matriz.
    int SetMatrix(Matrix<T> *input);

    ///@brief Asigna elemento con seguridad.
    void SetElement(unsigned int i, unsigned int j, T val);

    ///@brief Asigna val a todos los elementos.
    void Assign(T val);


    // ## Obtención de valores. ##
    T** GetMatrix();                            ///< Devuelve puntero a matriz.
    std::vector<T> GetRow(unsigned int row);    ///< Devuelve vector de fila especificada.
    unsigned int GetColumns();                  ///< Devuelve número de columnas.
    unsigned int GetRows();                     ///< Devuelve número de fila.as
    T Maximum();                                ///< Devuelve valor máximo dentro de matriz.
    bool IsOk();                                ///< Devuelve status.

    ///@brief Acceso seguro a elementos.
    T At(unsigned int i, unsigned int j);
    T*& operator[](const unsigned int pos);
};

template <class T> Matrix<T>::Matrix()
{
    m_ncols = 0;
    m_nrows = 0;
    m_status = 0;
    m_matrix = nullptr;
}
template <class T> Matrix<T>::Matrix(const Matrix<T> &other)
{
    m_matrix = nullptr;
    m_status = this->SetMatrix(other.m_nrows, other.m_ncols, other.m_matrix);
}
template <class T> Matrix<T>::Matrix(unsigned int rows, unsigned int cols, T** inputMatrix)
{
    m_matrix = nullptr;
    m_ncols = 0;
    m_nrows = 0;
    m_status = this->SetMatrix(rows, cols, inputMatrix);
}
template <class T> Matrix<T>::Matrix(unsigned int rows, unsigned int cols)
{
    m_matrix = nullptr;
    m_ncols = 0;
    m_nrows = 0;
    m_status = this->SetMatrix(rows, cols);
}
template <class T> Matrix<T>::~Matrix()
{
    this->Deallocate();
}
template <class T> void Matrix<T>::Deallocate()
{
    if (m_matrix != nullptr)
    {
        for (unsigned int i = 0; i < m_nrows; i++)
        {
            if (m_matrix[i] != nullptr)
                delete[] m_matrix[i];
        }
        delete[] m_matrix;
    }
}
template <class T> int Matrix<T>::SetMatrix(Matrix<T> *matrix)
{
    return this->SetMatrix(matrix->GetRows(), matrix->GetColumns(), matrix->GetMatrix());
}
template <class T> int Matrix<T>::SetMatrix(unsigned int rows, unsigned int cols, T** inputMatrix)
{
    this->Deallocate();
    m_nrows = rows;
    m_ncols = cols;
    try
    {
        m_matrix = new T*[m_nrows];
        for (unsigned int i = 0; i < m_nrows; i++)
            m_matrix[i] = new T[m_ncols];
    }
    catch (std::bad_alloc&)
    {
        return 1;
    }

    // Assigna valores.
    if (inputMatrix != nullptr)
    {
        for (unsigned int i = 0; i < m_nrows; i++)
        {
            for (unsigned int j = 0; j < m_ncols; j++)
                m_matrix[i][j] = inputMatrix[i][j];
        }
    }
    else
    {
        for (unsigned int i = 0; i < m_nrows; i++)
        {
            for (unsigned int j = 0; j < m_ncols; j++)
                m_matrix[i][j] = 0;
        }
    }
    return 0;
}
template <class T> int Matrix<T>::SetMatrix(unsigned int rows, unsigned int cols)
{
    this->Deallocate();
    m_nrows = rows;
    m_ncols = cols;
    try
    {
        m_matrix = new T*[m_nrows];
        for (unsigned int i = 0; i < m_nrows; i++)
            m_matrix[i] = new T[m_ncols];
    }
    catch (std::bad_alloc&)
    {
        return 1;
    }
    return 0;
}
template <class T> void Matrix<T>::Assign(T val)
{
    for (unsigned i = 0; i < m_nrows; i++)
    {
        for (unsigned j = 0; j < m_ncols; j++)
            m_matrix[i][j] = val;
    }
}
template <class T> T** Matrix<T>::GetMatrix()
{
    return m_matrix;
}
template <class T> void Matrix<T>::SetElement(unsigned int i, unsigned int j, T val)
{
    if ((i < m_nrows && i >= 0) && (j < m_ncols && j >= 0))
        m_matrix[i][j] = val;
}
template <class T> T Matrix<T>::At(unsigned int i, unsigned int j)
{
    if ((i < m_nrows && i >= 0) && (j < m_ncols && j >= 0))
        return m_matrix[i][j];
    else
        return 0;
}
template <class T> std::vector<T> Matrix<T>::GetRow(unsigned int fil)
{
    std::vector<T> out;
    for (int j = 0; j < m_ncols; j++)
        out.push_back(m_matrix[fil][j]);
    return out;
}
template <class T> unsigned int Matrix<T>::GetColumns()
{
    return m_ncols;
}
template <class T> unsigned int Matrix<T>::GetRows()
{
    return m_nrows;
}
template <class T> T Matrix<T>::Maximum()
{
    std::vector<T> tmp;
    for (unsigned i = 0; i < m_nrows; i++)
        tmp.push_back(max_element(m_matrix[i], m_matrix[i] + m_ncols));
    return max_element(tmp.begin(), tmp.end());
}
template <class T> bool Matrix<T>::IsOk()
{
    if (m_status == 0)
        return true;
    else
        return false;
}
template <class T> T*& Matrix<T>::operator[](const unsigned int pos)
{
    return m_matrix[pos];
}

/****************************
*                           *
*       Coordenadas         *
*                           *
****************************/

enum CoordLbl
{
	COORD_X, COORD_Y
};

/**
* @class Coord
* @brief Almacenamiento de coordenadas 2D.
**/
template <class T> class Coord
{
public:
    T m_x, m_y;
    Coord();
    Coord(T x, T y);
    Coord& operator=(const Coord& other);
    bool operator==(const Coord& other);
    bool operator!=(const Coord& other);
    T GetX();
    T GetY();
};

template <class T> Coord<T>::Coord()
{
    m_x = 0;
    m_y = 0;
}
template <class T> Coord<T>::Coord(T x, T y)
{
    m_x = x;
    m_y = y;
}
template <class T> Coord<T>& Coord<T>::operator=(const Coord<T>& other)
{
    m_x = other.m_x;
    m_y = other.m_y;
    return *this;
}
template <class T> bool Coord<T>::operator==(const Coord<T>& other)
{
    if (other.m_x == m_x && other.m_y == m_y)
        return true;
    else
        return false;
}
template <class T> bool Coord<T>::operator!=(const Coord<T>& other)
{
    if (other.m_x == m_x && other.m_y == m_y)
        return false;
    else
        return true;
}
template <class T> T Coord<T>::GetX()
{
    return m_x;
}
template <class T> T Coord<T>::GetY()
{
    return m_y;
}

template <class N> std::vector<N> aux_coordvec_to_vec(std::vector<Coord<N>> in, CoordLbl c)
{
	std::vector<N> out;
	for (unsigned i = 0; i < in.size(); ++i)
	{
		if (c == COORD_X)
			out.push_back(in[i].GetX());
		else
			out.push_back(in[i].GetY());
	}
	return out;
}

/**
* @class Coord3D
* @brief Almacenamiento de coordenadas 3D.
**/
template <class T> class Coord3D
{
public:
    T m_x, m_y, m_z;
    Coord3D();
    Coord3D(T x, T y, T z);
    Coord3D& operator=(const Coord3D& other);
    Coord3D& operator=(const Coord<T>& other);
    bool operator==(const Coord3D& other);
    bool operator!=(const Coord3D& other);
    T GetX();
    T GetY();
    T GetZ();
    Coord<T> SubXY();
    Coord<T> SubYZ();
    Coord<T> SubXZ();
};

template <class T> Coord3D<T>::Coord3D()
{
    m_x = 0;
    m_y = 0;
    m_z = 0;
}
template <class T> Coord3D<T>::Coord3D(T x, T y, T z)
{
    m_x = x;
    m_y = y;
    m_z = z;
}
template <class T> Coord3D<T>& Coord3D<T>::operator=(const Coord3D<T>& other)
{
    m_x = other.m_x;
    m_y = other.m_y;
    m_z = other.m_z;
    return *this;
}
template <class T> Coord3D<T>& Coord3D<T>::operator=(const Coord<T>& other)
{
    m_x = other.m_x;
    m_y = other.m_y;
    m_z = 0;
    return *this;
}
template <class T> bool Coord3D<T>::operator==(const Coord3D<T>& other)
{
    if (other.m_x == m_x && other.m_y == m_y && other.m_z == m_z)
        return true;
    else
        return false;
}
template <class T> bool Coord3D<T>::operator!=(const Coord3D<T>& other)
{
    if (other.m_x == m_x && other.m_y == m_y && other.m_z == m_z)
        return false;
    else
        return true;
}
template <class T> T Coord3D<T>::GetX()
{
    return m_x;
}
template <class T> T Coord3D<T>::GetY()
{
    return m_y;
}
template <class T> T Coord3D<T>::GetZ()
{
    return m_z;
}
template <class T> Coord<T> Coord3D<T>::SubXY()
{
    return Coord<T>(m_x, m_y);
}
template <class T> Coord<T> Coord3D<T>::SubYZ()
{
    return Coord<T>(m_y, m_z);
}
template <class T> Coord<T> Coord3D<T>::SubXZ()
{
    return Coord<T>(m_x, m_z);
}

#endif
