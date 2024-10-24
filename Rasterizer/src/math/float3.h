#pragma once

#include <ostream>

struct float3 
{
    float3();
    float3(float x, float y, float z);
    float3(const struct float4& other);

    union
    {
        struct
        {
            float x;
            float y;
            float z;
        };

        struct
        {
            float r;
            float g;
            float b;
        };
    };

    float3 operator+(const float3& other) const;
    float3 operator-(const float3& other) const;
    float3 operator*(const float3& other) const;
    float3 operator*(float scalar) const;
    float3 operator/(float scalar) const;
    float3& operator+=(const float3& other);

    friend float3 operator-(const float3& other);

    void Normalize();
    float3 Normalized() const;
    bool IsNormalized() const;
    void Clamp(float min = 0.0f, float max = 1.0f);
    float3 Clamped(float min = 0.0f, float max = 1.0f) const;
    float Magnitude() const;

    static float3 Reflect(const float3& incident, const float3& normal);
    static float Dot(const float3& a, const float3& b);
    static float3 Cross(const float3& a, const float3& b);
    static float AngleRad(const float3& a, const float3& b);
    static float AngleDeg(const float3& a, const float3& b);

    friend std::ostream& operator<<(std::ostream& os, const float3& v);
};