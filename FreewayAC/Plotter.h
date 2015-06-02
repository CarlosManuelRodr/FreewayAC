/**
* @file Plotter.h
* @brief Graficador interno. Escribe archivos en BMP.
* @author Carlos Manuel Rodríguez Martínez
* @date 1/06/2015
*/

#ifndef _MatrixClass
#define _MatrixClass

#include <vector>
#include <algorithm>

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


    // ## Obtención de valores. ##
    T** GetMatrix();
    std::vector<T> GetRow(unsigned int row);
    unsigned int GetColumns();
    unsigned int GetRows();
	T Maximum();

    ///@brief Acceso seguro a elementos.
    T At(unsigned int i, unsigned int j);
    T*& operator[](const unsigned int pos);
};

template <class T> Matrix<T>::Matrix()
{
    m_ncols = 0;
    m_nrows = 0;
    m_matrix = nullptr;
}
template <class T> Matrix<T>::Matrix(const Matrix<T> &other)
{
    m_matrix = nullptr;
    this->SetMatrix(other.m_nrows, other.m_ncols, other.m_matrix);
}
template <class T> Matrix<T>::Matrix(unsigned int rows, unsigned int cols, T** inputMatrix)
{
    m_matrix = nullptr;
    m_ncols = 0;
    m_nrows = 0;
    this->SetMatrix(rows, cols, inputMatrix);
}
template <class T> Matrix<T>::Matrix(unsigned int rows, unsigned int cols)
{
    m_matrix = nullptr;
    m_ncols = 0;
    m_nrows = 0;
    this->SetMatrix(rows, cols);
}
template <class T> Matrix<T>::~Matrix()
{
    this->Deallocate();
}
template <class T> void Matrix<T>::Deallocate()
{
    if (m_matrix != nullptr)
    {
        for (unsigned int i = 0; i<m_nrows; i++)
        {
            if (m_matrix[i] != nullptr) delete[] m_matrix[i];
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
        for (unsigned int i = 0; i<m_nrows; i++)
        {
            m_matrix[i] = new T[m_ncols];
        }
    }
    catch (std::bad_alloc& ba)
    {
        return 1;
    }

    // Assigna valores.
    if (inputMatrix != nullptr)
    {
        for (unsigned int i = 0; i<m_nrows; i++)
        {
            for (unsigned int j = 0; j<m_ncols; j++)
            {
                m_matrix[i][j] = inputMatrix[i][j];
            }
        }
    }
    else
    {
        for (unsigned int i = 0; i<m_nrows; i++)
        {
            for (unsigned int j = 0; j<m_ncols; j++)
            {
                m_matrix[i][j] = 0;
            }
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
        for (unsigned int i = 0; i<m_nrows; i++)
        {
            m_matrix[i] = new T[m_ncols];
        }
    }
    catch (std::bad_alloc& ba)
    {
        return 1;
    }
    return 0;
}
template <class T> T** Matrix<T>::GetMatrix()
{
    return m_matrix;
}
template <class T> void Matrix<T>::SetElement(unsigned int i, unsigned int j, T val)
{
    if ((i<m_nrows && i >= 0) && (j<m_ncols && j >= 0))
    {
        m_matrix[i][j] = val;
    }
}
template <class T> T Matrix<T>::At(unsigned int i, unsigned int j)
{
    if ((i<m_nrows && i >= 0) && (j<m_ncols && j >= 0))
    {
        return m_matrix[i][j];
    }
    else return 0;
}
template <class T> std::vector<T> Matrix<T>::GetRow(unsigned int fil)
{
    std::vector<T> out;
    for (int j = 0; j<m_ncols; j++)
    {
        out.push_back(m_matrix[fil][j]);
    }
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
	vector<T> tmp;
	for (unsigned i = 0; i < m_nrows; i++)
		tmp.push_back(max_element(m_matrix[i], m_matrix[i] + m_ncols));
	return max_element(tmp.begin(), tmp.end());
}
template <class T> T*& Matrix<T>::operator[](const unsigned int pos)
{
    return m_matrix[pos];
}

/****************************
*                           *
*     Paleta de colores     *
*                           *
****************************/

/**
* @enum STYLES
* @brief Estilos de paleta de colores.
*/
enum STYLES
{
	SUMMER_DAY,
	COOL_BLUE,
	HARD_RED,
	BLACK_AND_WHITE,
	PASTEL,
	PSYCH_EXPERIENCE,
	VIVID_COLORS,
	NONE
};

/****************************
*                           *
*  Funciones para graficar  *
*                           *
****************************/

void Plot(std::vector<int> &data, const std::string &filename, const unsigned &height = 30, 
	      const bool &normalize = false, const STYLES &style = SUMMER_DAY);

void Plot(Matrix<int> &data, const std::string &filename, const bool &normalize = false, 
	      const STYLES &style = SUMMER_DAY);

#endif