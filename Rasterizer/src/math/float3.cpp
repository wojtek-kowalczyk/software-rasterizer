#include "float3.h"

#include "float4.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cassert>


float3::float3() 
    : x(0), y(0), z(0)
{
}

float3::float3(float x, float y, float z)
    : x(x), y(y), z(z)
{
}

float3::float3(const float4& other)
    : x(other.x), y(other.y), z(other.z)
{
}

float3 float3::operator+(const float3 &other) const
{
    float3 result = *this;

    result.x += other.x;
    result.y += other.y;
    result.z += other.z;

    return result;
}

float3 float3::operator-(const float3 &other) const
{
    float3 result = *this;

    result.x -= other.x;
    result.y -= other.y;
    result.z -= other.z;

    return result;
}

float3 float3::operator*(const float3& other) const
{
    return float3(x * other.x, y * other.y, z * other.z);
}

float3 float3::operator*(float scalar) const
{
    float3 result = *this;

    result.x *= scalar;
    result.y *= scalar;
    result.z *= scalar;

    return result;
}

float3 float3::operator/(float scalar) const
{
    float3 result = *this;

    result.x /= scalar;
    result.y /= scalar;
    result.z /= scalar;

	return result;
}

float3& float3::operator+=(const float3& other)
{
    x += other.x;
	y += other.y;
	z += other.z;

	return *this;
}

float3 operator-(const float3& other)
{
    return float3(-other.x, -other.y, -other.z);
}

std::ostream& operator<<(std::ostream& os, const float3& v)
{
    os << '(' << v.x << ", " << v.y << ", " << v.z << ')';

    return os;
}


void float3::Normalize() 
{
    const float mag = Magnitude();
    assert(mag != 0);
    x /= mag;
    y /= mag;
    z /= mag;
}

float3 float3::Normalized() const
{
    float3 normalized = *this;
    normalized.Normalize();

    return normalized;
}

#include <iostream>

float float3::Magnitude() const
{
    float tmp = x*x + y*y + z*z;
    assert(tmp >= 0);

    if (tmp == 0)
    {
        std::cout << "magnitude called on 0 vector, make sure it's not a mistake!\n";
    }

    return sqrt(tmp);
}

bool float3::IsNormalized() const
{
    return Magnitude() - 1.0f < 0.0001f;
}

void float3::Clamp(float min, float max)
{
    x = fmin(fmax(x, min), max);
	y = fmin(fmax(y, min), max);
	z = fmin(fmax(z, min), max);
}

float3 float3::Clamped(float min, float max) const
{
    float3 copy = *this;
    copy.Clamp(min, max);

    return copy;
}

float3 float3::Reflect(const float3& incident, const float3& normal)
{
    float3 n = normal.Normalized();
    return incident - n * (2.0f * float3::Dot(incident, n));
}

float float3::Dot(const float3& a, const float3& b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

float3 float3::Cross(const float3& a, const float3& b) 
{
    // TODO : apparently this was wrong??? but why cross product of (0, -0.707, 0.707) and (0, 1, 0) is (-1, 0, 0) and not (1, 0, 0)???
    //return float3(
    //    (a.y * b.z) - (a.z * b.y),
    //    (a.x * b.z) - (a.z * b.x), // these operands are flipped, so that it works in left handed coorinate system... I think?
    //    (a.x * b.y) - (a.y * b.x)
    //);

    // TODO : aparently, the handedness of coordinate system doesn't matter? https://stackoverflow.com/questions/4820400/does-the-method-for-computing-the-cross-product-change-for-left-handed-coordinat
    // This is given for right hand. but accoring to one comment in the left hand it's flipped. (makes sense if you wave the hand in the air)
    // 
    // For right handed
    return float3(
        (a.y * b.z) - (a.z * b.y),
        (a.z * b.x) - (a.x * b.z),
        (a.x * b.y) - (a.y * b.x)
    );
}

float float3::AngleRad(const float3& a, const float3& b) 
{
    return acos(float3::Dot(a, b) / (a.Magnitude() * b.Magnitude()));
}

float float3::AngleDeg(const float3& a, const float3& b) 
{
    return AngleRad(a, b) * (float)(180.0 / M_PI);
}
