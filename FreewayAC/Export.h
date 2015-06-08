/**
* @file Plotter.h
* @brief Graficador interno. Escribe archivos en BMP.
* @author Carlos Manuel Rodr�guez Mart�nez
* @date 1/06/2015
*/

#ifndef _MatrixClass
#define _MatrixClass

#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>

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
    T** GetMatrix();
    std::vector<T> GetRow(unsigned int row);
    unsigned int GetColumns();
    unsigned int GetRows();
    T Maximum();
    bool IsOk();

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
    catch (std::bad_alloc&)
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
    BINARY_COLORS,
    NONE
};

/****************************
*                           *
*  Funciones para exportar  *
*                           *
****************************/

const BMPPixel white((char)255, (char)255, (char)255);
const BMPPixel black(0, 0, 0);

enum ExportFormat
{
	CSV, BMP
};

template <class N> int export_csv(std::vector<N> &data, const std::string &filename)
{
    std::ofstream file(filename.c_str(), std::ofstream::out);
    if (file.is_open())
    {
        for (unsigned i = 0; i < data.size(); ++i)
        {
            if (i - data.size() != 0)
                file << i << ", " << data[i] << std::endl;
            else
                file << i << ", " << data[i];
        }
        file.close();
        return 0;
    }
    else
    {
        std::cout << "Error: No se pudo crear archivo de salida." << std::endl;
        return 1;
    }
}

template <class N> int export_csv(std::vector<N> &data_1, std::vector<N> &data_2, const std::string &filename)
{
    if (data_1.size() == data_2.size())
    {
        std::ofstream file(filename.c_str(), std::ofstream::out);
        if (file.is_open())
        {
            for (unsigned i = 0; i < data_1.size(); ++i)
            {
                if (i - data_1.size() != 0)
                    file << data_1[i] << ", " << data_2[i] << std::endl;
                else
                    file << data_1[i] << ", " << data_2[i];
            }
            file.close();
            return 0;
        }
        else
        {
            std::cout << "Error: No se pudo crear archivo de salida." << std::endl;
            return 1;
        }
    }
    else
    {
        std::cout << "Error: Vectores de datos no coinciden en tamaño." << std::endl;
        return 1;
    }
}

template <class N> int export_plot(const std::vector<N> data, const std::string filename)
{
	unsigned coord_y;
	unsigned int size = data.size();
	double min_x = 0;
	double max_x = size;
	double min_y = static_cast<double>(*max_element(data.begin(), data.end()));
	double max_y = static_cast<double>(*min_element(data.begin(), data.end()));
	double y_factor = (max_y - min_y) / (size - 1);

	Matrix<bool> plot(size, size);
	plot.Assign(false);

	// Asigna valores a grafica.
	for (unsigned x = 0; x < size; x++)
	{
		coord_y = (unsigned)((max_y - data[x]) / y_factor);
		plot[x][coord_y] = true;
	}

	// Crea imagen.
	BMPWriter writer(filename.c_str(), size, size);
	if (writer.IsOpen())
	{
		vector<BMPPixel> bmp_data;
		for (unsigned j = 0; j < size; j++)
		{
			bmp_data.assign(size, white);
			for (unsigned i = 0; i < size; i++)
			{
				if (plot[i][j] == true)
					bmp_data[i] = black;
			}
			writer.WriteLine(bmp_data);
		}
		writer.CloseBMP();
		return 0;
	}
	else
		return 1;
}

template <class N> int export_data(std::vector<N> &data_1, std::vector<N> &data_2, const std::string &filename,
	                               ExportFormat &format)
{
	switch (format)
	{
	case CSV:
		return export_csv(data_1, data_2, aux_replace_extension(filename, "csv"));
		break;
	case BMP:
		return export_plot(data_2, aux_replace_extension(filename, "bmp"));
		break;
	default:
		return 1;
	}
}

template <class N> int export_data(std::vector<N> &data, const std::string &filename, ExportFormat &format)
{
	switch (format)
	{
	case CSV:
		return export_csv(data, aux_replace_extension(filename, "csv"));
		break;
	case BMP:
		return export_plot(data, aux_replace_extension(filename, "bmp"));
		break;
	default:
		return 1;
	}
}

int export_map(std::vector<int> &data, const std::string &filename, const unsigned &height = 30,
               const bool &normalize = false, const STYLES &style = SUMMER_DAY);

int export_map(Matrix<int> &data, const std::string &filename, const bool &normalize = false,
               const STYLES &style = SUMMER_DAY);

#endif
