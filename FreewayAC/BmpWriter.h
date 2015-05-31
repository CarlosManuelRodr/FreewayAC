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
#include <cstdint>

/**
* @struct BMPHeader
* @brief Encabezado del archivo BMP.
*/
struct BMPHeader
{
    uint16_t identifier;
    uint32_t size;
    uint16_t appSpecific1;
    uint16_t appSpecific2;
    uint32_t bitmapData;
};

/**
* @struct DIBHeader
* @brief Otro encabezado usado por los archivos BMP que contiene parámetros.
*/
struct DIBHeader
{
    uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t nPlanes;
    uint16_t colorDepth;
    uint32_t compression;
    uint32_t bmpBytes;
    int32_t hRes;
    int32_t vRes;
    uint32_t nColors;
    uint32_t nImpColors;
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