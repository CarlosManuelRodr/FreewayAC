/**
* @file Export.h
* @brief Funciones para exportar datos.
* @author Carlos Manuel Rodríguez Martínez
* @date 1/06/2015
*/

#ifndef _MatrixClass
#define _MatrixClass

#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "Auxiliar.h"


/****************************
*                           *
*     Paleta de colores     *
*                           *
****************************/

/**
* @enum Styles
* @brief Estilos de paleta de colores.
*/
enum Styles
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

/**
* @enum ExportFormat
* @brief Formatos para exportar archivos.
*/
enum ExportFormat
{
    CSV, BMP
};

/**
* @brief Exporta datos a archivo csv.
* @param data Lista de datos a exportar.
* @param filename Archivo a exportar los datos.
*/
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

/**
* @brief Exporta datos a archivo csv.
* @param data1 Lista de datos a exportar.
* @param data2 Lista de datos a exportar.
* @param filename Archivo a exportar los datos.
*/
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

/**
* @brief Exporta datos a archivo csv.
* @param data Lista de datos a exportar.
* @param filename Archivo a exportar los datos.
*/
template <class N> int export_csv(std::vector<Coord<N>> &data, const std::string &filename)
{
    std::vector<N> data1, data2;
    for (unsigned i = 0; i < data.size(); ++i)
    {
        data1.push_back(data[i].GetX());
        data2.push_back(data[i].GetY());
    }
    return export_csv(data1, data2, filename);
}

/**
* @brief Exporta datos como grafica en bmp.
* @param data Lista de datos a exportar.
* @param filename Archivo a exportar los datos.
*/
template <class N> int export_plot(const std::vector<N> data, const std::string filename)
{
    unsigned coord_y;
    unsigned int size = data.size();
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
        std::vector<BMPPixel> bmp_data;
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

/**
* @brief Exporta datos como grafica en bmp.
* @param data_1 Lista de datos de eje horizontal.
* @param data_2 Lista de datos de eje vertical.
* @param filename Archivo a exportar los datos.
*/
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
               const bool &normalize = false, const Styles &style = SUMMER_DAY);

int export_map(Matrix<int> &data, const std::string &filename, const bool &normalize = false,
               const Styles &style = SUMMER_DAY);

#endif
