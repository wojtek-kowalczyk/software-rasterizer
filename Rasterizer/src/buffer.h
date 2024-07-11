#pragma once

#include <cstdint>

class Buffer 
{
public:
    Buffer(unsigned short width, unsigned short height);
    ~Buffer();

    void ClearColor(uint32_t color);
    void SaveTGAFile(const char* filename);
    void ReadTGAFromFile(const char* filename);
    void* Data() const { return (void*)m_colorBuffer; }
    
    unsigned short GetWidth() const { return m_width; }
    unsigned short GetHeight() const { return m_height; }
    float GetAspectRatio() const { return (float)m_width / m_height; }
    
    uint32_t& ColorAt(int x, int y)         { return m_colorBuffer[y * m_width + x]; }
    uint32_t ColorAt(int x, int y) const    { return m_colorBuffer[y * m_width + x]; }

    float& DepthAt(int x, int y)            { return m_depthBuffer[y * m_width + x]; }
    float DepthAt(int x, int y) const       { return m_depthBuffer[y * m_width + x]; }

private:
    uint32_t* m_colorBuffer;
    float* m_depthBuffer;
    unsigned short m_width;
    unsigned short m_height;
};