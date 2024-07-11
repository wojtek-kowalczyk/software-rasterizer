#include "float4.h"

#include "float3.h"

#include <math.h>
#include <cassert>

float4 float4::operator+(const float4& other) const
{
    float4 result = *this;

    result.x += other.x;
    result.y += other.y;
    result.z += other.z;
    result.w += other.w;

    return result;
}

float4 float4::operator-(const float4& other) const
{
    float4 result = *this;

    result.x -= other.x;
    result.y -= other.y;
    result.z -= other.z;
    result.w -= other.w;

    return result;
}

float4 float4::operator*(float scalar) const
{
    float4 result = *this;

    result.x *= scalar;
    result.y *= scalar;
    result.z *= scalar;
    result.w *= scalar;

    return result;
}

float4 float4::operator/(float scalar) const
{
    float4 result = *this;

    result.x /= scalar;
    result.y /= scalar;
    result.z /= scalar;
    result.w /= scalar;

    return result;
}

std::ostream& operator<<(std::ostream& os, const float4& v)
{
    os << '(' << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ')';

    return os;
}

void float4::Normalize()
{
    const float mag = Magnitude();
    x /= mag;
    y /= mag;
    z /= mag;
    w /= mag;
}

float4 float4::Normalized() const
{
    float4 normalized = *this;
    normalized.Normalize();

    return normalized;
}

float float4::Magnitude() const
{
    float tmp = x * x + y * y + z * z + w * w;
    assert(tmp > 0);

    return sqrt(tmp);
}

bool float4::IsNormalized() const
{
    return Magnitude() - 1.0f < 0.0001f;
}

float float4::Dot(const float4& a, const float4& b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}
