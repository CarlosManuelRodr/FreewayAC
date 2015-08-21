#include "BmpWriter.h"
#include "Export.h"
#include <cmath>
using namespace std;


/****************************
*                           *
*  Funciones para exportar  *
*                           *
****************************/

int export_map(vector<int> data, string filename, const unsigned height)
{
    unsigned width = data.size();
    BMPWriter writer(filename.c_str(), width, height);
    vector<BMPPixel> bmp_data;

    if (writer.IsOpen())
    {
        bmp_data.assign(width, white);
        for (unsigned i = 0; i < width; ++i)
        {
            if (data[i] == 1)
                bmp_data[i] = black;
            else
                bmp_data[i] = white;
        }

        for (int i = height - 1; i >= 0; --i)
            writer.WriteLine(bmp_data);

        writer.CloseBMP();
        return 0;
    }
    else
        return 1;
}
int export_map(Matrix<int> data, const string filename)
{
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
				if (data[i][j] == 1)
					bmp_data[i] = black;
				else
					bmp_data[i] = white;
            }
            writer.WriteLine(bmp_data);
        }
        writer.CloseBMP();
        return 0;
    }
    else
        return 1;
}
