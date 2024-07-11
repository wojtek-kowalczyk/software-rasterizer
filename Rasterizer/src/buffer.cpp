#include "buffer.h"

#include <cstdint>
#include <assert.h>
#include <cstdio>
#include <limits>

// Ignore visual studio warning
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

Buffer::Buffer(unsigned short width, unsigned short height) 
    : m_width(width), m_height(height) 
{
    m_colorBuffer = new uint32_t[width * height] { 0 };
    assert(m_colorBuffer != nullptr && "Color buffer must not be nullptr!");

    m_depthBuffer = new float[width * height];
    assert(m_depthBuffer != nullptr && "Depth must not be nullptr");
    for (int i = 0; i < m_width * m_height; i++)
    {
        // TODO : what is a good initial value for the depth buffer?
        m_depthBuffer[i] = std::numeric_limits<float>::max();
    }
}

Buffer::~Buffer() 
{
    delete[] m_colorBuffer;
    m_colorBuffer = nullptr;
    delete[] m_depthBuffer;
    m_depthBuffer = nullptr;
}

void Buffer::ClearColor(uint32_t argb) 
{
    for (int i = 0; i < m_width * m_height; i++)
    {
        m_colorBuffer[i] = argb;
    }
}

void Buffer::SaveTGAFile(const char* filename) 
{
    unsigned short header[9] = {
        0x0000, 0x0002, 0x0000, 0x0000, 0x0000, 0x0000,
        m_width, m_height,
        0x0820
    };

    FILE* file = fopen(filename, "wb+");
    assert(file != nullptr && "failed to open file for writing");
    fwrite(header, 2, 9, file);
    fwrite(m_colorBuffer, 4, m_width * m_height, file);
    fclose(file);
}

// #include <iostream>

void Buffer::ReadTGAFromFile(const char* filename) 
{
    FILE* file = fopen(filename, "rb");
    assert(file != nullptr && "Failed to open file for reading");

    unsigned short header[9] = {0};
    int read = fread(&header, 2, 9, file);
    // std::cout << read << " read bytes\n" << "error: " << ferror(file) << ", eof: " << feof(file) << '\n';
    m_width = header[6];
    m_height = header[7];
    // std::cout << m_width <<'\n';
    // std::cout << m_height <<'\n';

    delete [] m_colorBuffer;
    m_colorBuffer = new uint32_t[m_width*m_height];
    uint8_t* buffer = new uint8_t[3 * m_width * m_height]; // skip alpha
    fread(buffer, sizeof(uint8_t) * 3, m_width * m_height, file);
    int pixel = 0;
    for (int i = 0; i < 3 * m_width * m_height; i+=3) {
        uint8_t blue = buffer[i+0];
        uint8_t green = buffer[i+1];
        uint8_t red = buffer[i+2];
        uint32_t color = (0xff << 24) | (red << 16) | (green << 8) | (blue << 0);
        m_colorBuffer[pixel] = color;
        pixel++;
    }

    delete [] buffer;
    
    fclose(file);
}
