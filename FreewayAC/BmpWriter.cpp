#include <fstream>
#include "BmpWriter.h"
#include <string>
using namespace std;

// Binary Ops.
char* ToByte(void* ptr)
{
    return static_cast<char*>(ptr);
}

bmp_uint16 SwitchEndianess16(bmp_uint16 data)
{
    bmp_uint16 temp;
    bmp_uint16 out = 0x0000;
    bmp_uint16 pos = 0x00FF;

    temp = data & pos;
    temp <<= 8;
    out |= temp;
    pos <<= 8;

    temp = data & pos;
    temp >>= 8;
    out |= temp;

    return out;
}

bmp_uint32 SwitchEndianess32(bmp_uint32 data)
{
    bmp_uint32 temp;
    bmp_uint32 out = 0x00000000;
    bmp_uint32 pos = 0x000000FF;

    temp = data & pos;
    temp <<= 24;
    out |= temp;
    pos <<= 8;
    temp = data & pos;
    temp <<= 8;
    out |= temp;
    pos <<= 8;
    temp = data & pos;
    temp >>= 8;
    out |= temp;
    pos <<= 8;
    temp = data & pos;
    temp >>= 24;
    out |= temp;

    return out;
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

BMPWriter::BMPWriter(const char* filepath, unsigned int width, unsigned int height)
{
    // Create header.
    m_indexHeight = 0;
    m_width = width;
    m_height = height;
    m_dataSize = width*height;
    m_bmpHdr = new BMPHeader;
    m_dibHdr = new DIBHeader;

    m_bmpHdr->identifier = SwitchEndianess16(0x424d);    // Windows.
    m_bmpHdr->appSpecific1 = 0x0000;
    m_bmpHdr->appSpecific2 = 0x0000;

    m_dibHdr->width = width;
    m_dibHdr->height = height;
    m_dibHdr->nPlanes = SwitchEndianess16(0x0100);
    m_dibHdr->colorDepth = SwitchEndianess16(0x1800);
    m_dibHdr->compression = 0x00000000;
    m_dibHdr->bmpBytes = SwitchEndianess32(0x10000000);
    m_dibHdr->hRes = SwitchEndianess32(0x130B0000);
    m_dibHdr->vRes = SwitchEndianess32(0x130B0000);
    m_dibHdr->nColors = 0x00000000;
    m_dibHdr->nImpColors = 0x00000000;

    unsigned int bmpSize = 0;
    unsigned int offsetData = 54;
    m_paddingBytes = m_width % 4;
    // Calculate file size.
    bmpSize += 14;        //BMPHeader size.
    bmpSize += 40;        //DIBHeader size
    bmpSize += 3*width*height;
    bmpSize += m_height*m_paddingBytes;
    m_bmpHdr->size = bmpSize;
    m_bmpHdr->bitmapData = offsetData;
    m_dibHdr->headerSize = 40;        //DIBHeader size

    // Write header.
    m_file.open( filepath, ios::out | ios::binary );
    m_file.write( ToByte(&m_bmpHdr->identifier), 2 );
    m_file.write( ToByte(&m_bmpHdr->size), 4);
    m_file.write( ToByte(&m_bmpHdr->appSpecific1), 2 );
    m_file.write( ToByte(&m_bmpHdr->appSpecific2), 2 );
    m_file.write( ToByte(&m_bmpHdr->bitmapData), 4 );
    m_file.write( ToByte(&m_dibHdr->headerSize), 4 );
    m_file.write( ToByte(&m_dibHdr->width), 4 );
    m_file.write( ToByte(&m_dibHdr->height), 4 );
    m_file.write( ToByte(&m_dibHdr->nPlanes), 2 );
    m_file.write( ToByte(&m_dibHdr->colorDepth), 2 );
    m_file.write( ToByte(&m_dibHdr->compression), 4 );
    m_file.write( ToByte(&m_dibHdr->bmpBytes), 4 );
    m_file.write( ToByte(&m_dibHdr->hRes), 4 );
    m_file.write( ToByte(&m_dibHdr->vRes), 4 );
    m_file.write( ToByte(&m_dibHdr->nColors), 4 );
    m_file.write( ToByte(&m_dibHdr->nImpColors), 4 );
}
BMPWriter::~BMPWriter()
{
    delete m_bmpHdr;
    delete m_dibHdr;
}
void BMPWriter::WriteLine(BMPPixel* data)
{
    // We are going to write the file line by line.
    if(m_indexHeight < m_height)
    {
        for(unsigned int i=0; i<m_width; i++)
        {
            m_file.write(&data[i].b, 1);
            m_file.write(&data[i].g, 1);
            m_file.write(&data[i].r, 1);
        }
        if(m_paddingBytes == 1)
        {
            char padding = 0x00;
            m_file.write(ToByte(&padding), 1);
        }
        if(m_paddingBytes == 2)
        {
            short padding = 0x0000;
            m_file.write(ToByte(&padding), 2);
        }
        if(m_paddingBytes == 3)
        {
            unsigned int padding = 0x00000000;
            m_file.write(ToByte(&padding), 3);
        }
    }
    m_indexHeight++;
}
void BMPWriter::CloseBMP()
{
    m_file.close();
}