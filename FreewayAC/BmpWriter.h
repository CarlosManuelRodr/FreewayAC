/**
* @file BmpWriter.h
* @brief Escritor de archivos BMP simple.
* @author Carlos Manuel Rodriguez Martinez
* @date 7/18/2012
*/

#pragma once
#ifndef __bmpWriter
#define __bmpWriter

#include <fstream>

typedef short unsigned int bmp_uint16;
typedef unsigned int bmp_uint32;
typedef int bmp_int32;

/**
* @struct BMPHeader
* @brief Encabezado del archivo BMP.
*/
struct BMPHeader
{
    bmp_uint16 identifier;
    bmp_uint32 size;
    bmp_uint16 appSpecific1;
    bmp_uint16 appSpecific2;
    bmp_uint32 bitmapData;
};

/**
* @struct DIBHeader
* @brief Otro encabezado usado por los archivos BMP que contiene parámetros.
*/
struct DIBHeader
{
    bmp_uint32 headerSize;
    bmp_int32 width;
    bmp_int32 height;
    bmp_uint16 nPlanes;
    bmp_uint16 colorDepth;
    bmp_uint32 compression;
    bmp_uint32 bmpBytes;
    bmp_int32 hRes;
    bmp_int32 vRes;
    bmp_uint32 nColors;
    bmp_uint32 nImpColors;
};

/**
* @struct BMPPixel
* @brief Pixel simple RGB.
*/
class BMPPixel
{
public:
    char r, g, b;
    BMPPixel();
    BMPPixel(char mR, char mG, char mB);
    bool operator==(const BMPPixel &other);
};

/**
* @class BMPWriter
* @brief Clase para escribir archivos de mapas de bits de gran tamaño.
*/
class BMPWriter
{
    BMPHeader* m_bmpHdr;
    DIBHeader* m_dibHdr;
    std::ofstream m_file;
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_paddingBytes;
    int m_dataSize;
    unsigned int m_indexHeight;

public:
    ///@brief Constructor.
    ///@param filepath Ruta del archivo a guardar.
    ///@param width Tamaño horizontal de la imagen.
    ///@param height Tamaño vertical de la imagen.
    BMPWriter(const char* filepath, unsigned int width, unsigned int height);
    ~BMPWriter();

    ///@brief Writes BMP line.
    ///@param Array de pixeles a escribir. Las líneas se escriben de abajo a arriba.
    void WriteLine(BMPPixel* data);
    void CloseBMP();
};

#endif