#include <climits>
#include <iostream>
#include "BmpWriter.h"
using namespace std;

// Operaciones binarias.
char* to_byte(void* ptr)
{
    return static_cast<char*>(ptr);
}
template<typename T> T swap_endian(T u)
{
	static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");
	union
	{
		T u;
		unsigned char u8[sizeof(T)];
	} source, dest;
	source.u = u;

	for (size_t k = 0; k < sizeof(T); k++)
		dest.u8[k] = source.u8[sizeof(T) - k - 1];
	return dest.u;
}

BMPPixel::BMPPixel()
{
    r = 0;
    g = 0;
    b = 0;
}
BMPPixel::BMPPixel(char mR, char mG, char mB)
{
    r = mR;
    g = mG;
    b = mB;
}
bool BMPPixel::operator==(const BMPPixel &other)
{
    if(r == other.r && g == other.g && b == other.b) return true;
    else return false;
}

BMPWriter::BMPWriter(string filepath, unsigned int width, unsigned int height)
{
    // Crea encabezado.
    m_indexHeight = 0;
    m_width = width;
    m_height = height;
    m_dataSize = width*height;
    m_bmpHdr = new BMPHeader;
    m_dibHdr = new DIBHeader;

	m_bmpHdr->identifier = swap_endian<uint16_t>(0x424d);    // Windows.
    m_bmpHdr->appSpecific1 = 0x0000;
    m_bmpHdr->appSpecific2 = 0x0000;

    m_dibHdr->width = width;
    m_dibHdr->height = height;
	m_dibHdr->nPlanes = swap_endian<uint16_t>(0x0100);
	m_dibHdr->colorDepth = swap_endian<uint16_t>(0x1800);
    m_dibHdr->compression = 0x00000000;
	m_dibHdr->bmpBytes = swap_endian<uint32_t>(0x10000000);
	m_dibHdr->hRes = swap_endian<int32_t>(0x130B0000);
	m_dibHdr->vRes = swap_endian<int32_t>(0x130B0000);
    m_dibHdr->nColors = 0x00000000;
    m_dibHdr->nImpColors = 0x00000000;

    unsigned int bmpSize = 0;
    unsigned int offsetData = 54;
    m_paddingBytes = m_width % 4;

    // Calcula tamaño del archivo.
    bmpSize += 14;        //BMPHeader size.
    bmpSize += 40;        //DIBHeader size.
    bmpSize += 3*width*height;
    bmpSize += m_height*m_paddingBytes;
    m_bmpHdr->size = bmpSize;
    m_bmpHdr->bitmapData = offsetData;
    m_dibHdr->headerSize = 40;        //DIBHeader size.

    // Escribe encabezado.
    m_file.open(filepath.c_str(), ios::out | ios::binary);
    if (m_file.is_open())
    {
        m_file.write(to_byte(&m_bmpHdr->identifier), 2);
        m_file.write(to_byte(&m_bmpHdr->size), 4);
        m_file.write(to_byte(&m_bmpHdr->appSpecific1), 2);
        m_file.write(to_byte(&m_bmpHdr->appSpecific2), 2);
        m_file.write(to_byte(&m_bmpHdr->bitmapData), 4);
        m_file.write(to_byte(&m_dibHdr->headerSize), 4);
        m_file.write(to_byte(&m_dibHdr->width), 4);
        m_file.write(to_byte(&m_dibHdr->height), 4);
        m_file.write(to_byte(&m_dibHdr->nPlanes), 2);
        m_file.write(to_byte(&m_dibHdr->colorDepth), 2);
        m_file.write(to_byte(&m_dibHdr->compression), 4);
        m_file.write(to_byte(&m_dibHdr->bmpBytes), 4);
        m_file.write(to_byte(&m_dibHdr->hRes), 4);
        m_file.write(to_byte(&m_dibHdr->vRes), 4);
        m_file.write(to_byte(&m_dibHdr->nColors), 4);
        m_file.write(to_byte(&m_dibHdr->nImpColors), 4);
    }
    else
    {
        cout << "Error: No se puede crear archivo BMP." << endl;
    }
}
BMPWriter::~BMPWriter()
{
    delete m_bmpHdr;
    delete m_dibHdr;
}
void BMPWriter::WriteLine(BMPPixel* data)
{
    // Escribe línea individual del BMP.
    if(m_indexHeight < m_height)
    {
        for(unsigned int i = 0; i < m_width; i++)
        {
            m_file.write(&data[i].b, 1);
            m_file.write(&data[i].g, 1);
            m_file.write(&data[i].r, 1);
        }
        if(m_paddingBytes == 1)
        {
            char padding = 0x00;
            m_file.write(to_byte(&padding), 1);
        }
        if(m_paddingBytes == 2)
        {
            short padding = 0x0000;
            m_file.write(to_byte(&padding), 2);
        }
        if(m_paddingBytes == 3)
        {
            unsigned int padding = 0x00000000;
            m_file.write(to_byte(&padding), 3);
        }
    }
    m_indexHeight++;
}
void BMPWriter::WriteLine(std::vector<BMPPixel> data)
{
	if (!data.empty())
		WriteLine(&data[0]);
}
bool BMPWriter::IsOpen()
{
    return m_file.is_open();
}
void BMPWriter::CloseBMP()
{
    m_file.close();
}
