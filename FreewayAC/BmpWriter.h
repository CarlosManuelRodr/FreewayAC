#pragma once
#ifndef __bmpWriter
#define __bmpWriter

#include <fstream>

typedef short unsigned int bmp_uint16;
typedef unsigned int bmp_uint32;
typedef int bmp_int32;

struct BMPHeader
{
    bmp_uint16 identifier;
    bmp_uint32 size;
    bmp_uint16 appSpecific1;
    bmp_uint16 appSpecific2;
    bmp_uint32 bitmapData;
};

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

class BMPPixel
{
public:
    char r, g, b;
    BMPPixel();
    BMPPixel(char mR, char mG, char mB);
    bool operator==(const BMPPixel &other);
};

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
    BMPWriter(const char* filepath, unsigned int width, unsigned int height);
    ~BMPWriter();
    void WriteLine(BMPPixel* data);
    void CloseBMP();
};

#endif