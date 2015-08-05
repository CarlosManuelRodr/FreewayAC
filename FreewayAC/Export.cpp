#include "BmpWriter.h"
#include "Export.h"
#include <cmath>
using namespace std;

/****************************
*                           *
*     Paleta de colores     *
*                           *
****************************/

/**
* @class Palette
* @brief Paleta de colores.
**/
class Palette
{
    Styles m_style;
    unsigned m_red_int, m_green_int, m_blue_int;
    double m_red_mean, m_green_mean, m_blue_mean;
    double m_red_dev, m_green_dev, m_blue_dev;
    unsigned m_palette_size;
    vector<BMPPixel> m_color_palette;

    ///@brief Distribución normal. Se usa para calcular el color.
    ///@param x Posición.
    ///@param mean Media.
    ///@param std_dev Desviación estándar.
    double NormalDist(unsigned x, double mean, double std_dev);

    ///@brief Calcula color con la paleta de colores del estilo seleccionado.
    ///@param color_num Número a convertir en color.
    ///@return Color de paleta.
    BMPPixel CalcColor(unsigned color_num);

public:
    ///@brief Crea paleta. Por defecto usa estilo SUMMER_DAY.
    Palette();

    ///@brief Asigna parámetros dependiendo del estilo.
    ///@param style Estilo seleccionado.
    void SetStyle(Styles style);

    unsigned GetPaletteSize();

    BMPPixel operator[](const unsigned &i);
};

Palette::Palette()
{
    m_style = NONE;
    SetStyle(SUMMER_DAY);
}
double Palette::NormalDist(unsigned x, double mean, double std_dev)
{
    return exp(-(pow((double)x - mean, 2.0)/(2.0*pow(std_dev, 2.0))));
}
BMPPixel Palette::CalcColor(unsigned color_num)
{
    char r, g, b;
    r = static_cast<char>(m_red_int*NormalDist(color_num, m_red_mean, m_red_dev)
                          + m_red_int*NormalDist(color_num, (double)m_palette_size + m_red_mean, m_red_dev));
    g = static_cast<char>(m_green_int*NormalDist(color_num, m_green_mean, m_green_dev)
                          + m_green_int*NormalDist(color_num, (double)m_palette_size + m_green_mean, m_green_dev));
    b = static_cast<char>(m_blue_int*NormalDist(color_num, m_blue_mean, m_blue_dev)
                           + m_blue_int*NormalDist(color_num, (double)m_palette_size + m_blue_mean, m_blue_dev));

    return BMPPixel(r, g, b);
}
void Palette::SetStyle(Styles style)
{
    // Asigna parámetros de cada estilo de color.
    if (style != m_style)
    {
        switch (style)
        {
        case SUMMER_DAY:
            {
                m_red_int = 255;
                m_red_mean = 34;
                m_red_dev = 8;
                m_green_int = 201;
                m_green_mean = 28;
                m_green_dev = 12;
                m_blue_int = 255;
                m_blue_mean = 21;
                m_blue_dev = 9;
                m_palette_size = 60;
                break;
            }
        case COOL_BLUE:
            {
                m_red_int = 252;
                m_red_mean = 104;
                m_red_dev = 37;
                m_green_int = 255;
                m_green_mean = 96;
                m_green_dev = 34;
                m_blue_int = 255;
                m_blue_mean = 73;
                m_blue_dev = 40;
                m_palette_size = 300;
                break;
            }
        case HARD_RED:
            {
                m_red_int = 210;
                m_red_mean = 19;
                m_red_dev = 14;
                m_green_int = 102;
                m_green_mean = 26;
                m_green_dev = 10;
                m_blue_int = 97;
                m_blue_mean = 29;
                m_blue_dev = 10;
                m_palette_size = 60;
                break;
            }
        case BLACK_AND_WHITE:
            {
                m_red_int = 200;
                m_red_mean = 30;
                m_red_dev = 9;
                m_green_int = 200;
                m_green_mean = 30;
                m_green_dev = 9;
                m_blue_int = 200;
                m_blue_mean = 30;
                m_blue_dev = 9;
                m_palette_size = 60;
                break;
            }
        case PASTEL:
            {
                m_red_int = 205;
                m_red_mean = 23;
                m_red_dev = 14;
                m_green_int = 196;
                m_green_mean = 26;
                m_green_dev = 11;
                m_blue_int = 198;
                m_blue_mean = 38;
                m_blue_dev = 9;
                m_palette_size = 60;
                break;
            }
        case PSYCH_EXPERIENCE:
            {
                m_red_int = 126;
                m_red_mean = 3;
                m_red_dev = 11;
                m_green_int = 200;
                m_green_mean = 36;
                m_green_dev = 9;
                m_blue_int = 200;
                m_blue_mean = 24;
                m_blue_dev = 12;
                m_palette_size = 60;
                break;
            }
        case VIVID_COLORS:
            {
                m_red_int = 200;
                m_red_mean = 0;
                m_red_dev = 10;
                m_green_int = 200;
                m_green_mean = 17;
                m_green_dev = 10;
                m_blue_int = 200;
                m_blue_mean = 37;
                m_blue_dev = 10;
                m_palette_size = 60;
                break;
            }
        default:
            break;
        };

        // Crea paleta.
        m_color_palette.clear();
        for (unsigned i = 0; i < m_palette_size; ++i)
            m_color_palette.push_back(CalcColor(i));
    }
}
unsigned Palette::GetPaletteSize()
{
    return m_palette_size;
}
BMPPixel Palette::operator[](const unsigned &i)
{
    return m_color_palette[i % m_palette_size];
}

/****************************
*                           *
*  Funciones para exportar  *
*                           *
****************************/

int export_map(vector<int> data, string filename, const unsigned height,
               const bool normalize, const Styles style)
{
    Palette palette;
    if (style != BINARY_COLORS)
        palette.SetStyle(style);

    unsigned width = data.size();
    unsigned palette_size = palette.GetPaletteSize();
    BMPWriter writer(filename.c_str(), width, height);
    vector<BMPPixel> bmp_data;

    if (writer.IsOpen())
    {
        bmp_data.assign(width, white);
        if (normalize)
        {
            unsigned max = *max_element(data.begin(), data.end());
            for (unsigned i = 0; i < width; ++i)
            {
                if (style == BINARY_COLORS)
                {
                    if (data[i] == 1)
                        bmp_data[i] = black;
                    else
                        bmp_data[i] = white;
                }
                else
                    bmp_data[i] = palette[(unsigned)(palette_size*((double)data[i]/(double)max))];
            }
        }
        else
        {
            for (unsigned i = 0; i < width; ++i)
            {
                if (style == BINARY_COLORS)
                {
                    if (data[i] == 1)
                        bmp_data[i] = black;
                    else
                        bmp_data[i] = white;
                }
                else
                    bmp_data[i] = palette[data[i]];
            }
        }

        for (int i = height - 1; i >= 0; --i)
            writer.WriteLine(bmp_data);

        writer.CloseBMP();
        return 0;
    }
    else
        return 1;
}
int export_map(Matrix<int> data, const string filename, const Styles style)
{
    Palette palette;
    if (style != BINARY_COLORS)
        palette.SetStyle(style);

    unsigned width = data.GetColumns();
    unsigned height = data.GetRows();

    BMPWriter writer(filename.c_str(), width, height);
    vector<BMPPixel> bmp_data;

    if (writer.IsOpen())
    {
        bmp_data.assign(width, white);
        for(int i = height-1; i >= 0; --i)
        {
            for (unsigned j = 0; j < width; ++j)
            {
                if (style == BINARY_COLORS)
                {
                    if (data[i][j] == 1)
                        bmp_data[i] = black;
                    else
                        bmp_data[i] = white;
                }
                else
                    bmp_data[j] = palette[data[i][j]];
            }
            writer.WriteLine(bmp_data);
        }
        writer.CloseBMP();
        return 0;
    }
    else
        return 1;
}
