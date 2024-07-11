#pragma once

#include <ostream>

#include "float3.h"

struct float4
{
    union
    {
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };

        struct
        {
            float r;
            float g;
            float b;
            float a;
        };
    };

    float4 operator+(const float4& other) const;
    float4 operator-(const float4& other) const;
    float4 operator*(float scalar) const;
    float4 operator/(float scalar) const;

    void Normalize();
    float4 Normalized() const;
    float Magnitude() const;
    bool IsNormalized() const;

    static float Dot(const float4& a, const float4& b);

    friend std::ostream& operator<<(std::ostream& os, const float4& v);
};