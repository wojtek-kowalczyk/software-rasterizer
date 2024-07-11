#include "float4x4.h"
#include "float4.h"

#define _USE_MATH_DEFINES
#include <math.h>

float4x4::float4x4()
	: m00(0), m01(0), m02(0), m03(0), m10(0), m11(0), m12(0), m13(0), m20(0), m21(0), m22(0), m23(0), m30(0), m31(0), m32(0), m33(0)
{
}

float4x4::float4x4(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
    : m00(m00), m01(m01), m02(m02), m03(m03), m10(m10), m11(m11), m12(m12), m13(m13), m20(m20), m21(m21), m22(m22), m23(m23), m30(m30), m31(m31), m32(m32), m33(m33)
{
}

float4x4 float4x4::operator*(const float4x4& other) const
{
	float4x4 result;

	result.m00 = m00 * other.m00 + m01 * other.m10 + m02 * other.m20 + m03 * other.m30;
	result.m01 = m00 * other.m01 + m01 * other.m11 + m02 * other.m21 + m03 * other.m31;
	result.m02 = m00 * other.m02 + m01 * other.m12 + m02 * other.m22 + m03 * other.m32;
	result.m03 = m00 * other.m03 + m01 * other.m13 + m02 * other.m23 + m03 * other.m33;

	result.m10 = m10 * other.m00 + m11 * other.m10 + m12 * other.m20 + m13 * other.m30;
	result.m11 = m10 * other.m01 + m11 * other.m11 + m12 * other.m21 + m13 * other.m31;
	result.m12 = m10 * other.m02 + m11 * other.m12 + m12 * other.m22 + m13 * other.m32;
	result.m13 = m10 * other.m03 + m11 * other.m13 + m12 * other.m23 + m13 * other.m33;

	result.m20 = m20 * other.m00 + m21 * other.m10 + m22 * other.m20 + m23 * other.m30;
	result.m21 = m20 * other.m01 + m21 * other.m11 + m22 * other.m21 + m23 * other.m31;
	result.m22 = m20 * other.m02 + m21 * other.m12 + m22 * other.m22 + m23 * other.m32;
	result.m23 = m20 * other.m03 + m21 * other.m13 + m22 * other.m23 + m23 * other.m33;

	result.m30 = m30 * other.m00 + m31 * other.m10 + m32 * other.m20 + m33 * other.m30;
	result.m31 = m30 * other.m01 + m31 * other.m11 + m32 * other.m21 + m33 * other.m31;
	result.m32 = m30 * other.m02 + m31 * other.m12 + m32 * other.m22 + m33 * other.m32;
	result.m33 = m30 * other.m03 + m31 * other.m13 + m32 * other.m23 + m33 * other.m33;

	return result;
}

float4 float4x4::operator*(const float4& vector) const
{
	float4 result;

	result.x = m00 * vector.x + m01 * vector.y + m02 * vector.z + m03 * vector.w;
	result.y = m10 * vector.x + m11 * vector.y + m12 * vector.z + m13 * vector.w;
	result.z = m20 * vector.x + m21 * vector.y + m22 * vector.z + m23 * vector.w;
	result.w = m30 * vector.x + m31 * vector.y + m32 * vector.z + m33 * vector.w;

	return result;
}

float4 float4x4::operator*(const float3& vector) const
{
	float4 result;

	float4 temp = float4{vector.x, vector.y, vector.z, 1.0f};
	result = (*this) * temp;

	return result;
}

float4x4 float4x4::operator*(float scalar) const
{
	float4x4 result;

	result.m00 = m00 * scalar;
	result.m01 = m01 * scalar;
	result.m02 = m02 * scalar;
	result.m03 = m03 * scalar;

	result.m10 = m10 * scalar;
	result.m11 = m11 * scalar;
	result.m12 = m12 * scalar;
	result.m13 = m13 * scalar;

	result.m20 = m20 * scalar;
	result.m21 = m21 * scalar;
	result.m22 = m22 * scalar;
	result.m23 = m23 * scalar;

	result.m30 = m30 * scalar;
	result.m31 = m31 * scalar;
	result.m32 = m32 * scalar;
	result.m33 = m33 * scalar;

	return result;
}

void float4x4::Transpose()
{
	float4x4 temp = *this;

	m01 = temp.m10;
	m02 = temp.m20;
	m03 = temp.m30;

	m10 = temp.m01;
	m12 = temp.m21;
	m13 = temp.m31;

	m20 = temp.m02;
	m21 = temp.m12;
	m23 = temp.m32;

	m30 = temp.m03;
	m31 = temp.m13;
	m32 = temp.m23;
}

float4x4 float4x4::Translate(float3 translation)
{
	float4x4 result = float4x4::Identity();

	result.m03 = translation.x;
	result.m13 = translation.y;
	result.m23 = translation.z;

	return result;
}

float4x4 float4x4::Rotate(float angle, float3 axis)
{
	const float s = sinf(angle * (float)M_PI / 180.0f);
	const float c = cosf(angle * (float)M_PI / 180.0f);
	
	axis.Normalize();

	float4x4 result = float4x4::Identity();
	result.row0 = float4{axis.x * axis.x * (1 - c) + c, axis.y * axis.x * (1 - c) + axis.z * s, axis.x * axis.z * (1 - c) - axis.y * s, 0};
	result.row1 = float4{axis.x * axis.y * (1 - c) - axis.z * s, axis.y * axis.y * (1 - c) + c, axis.y * axis.z * (1 - c) + axis.x * s, 0};
	result.row2 = float4{axis.x * axis.z * (1 - c) + axis.y * s, axis.y * axis.z * (1 - c) - axis.x * s, axis.z * axis.z * (1 - c) + c, 0};
	result.row3 = float4{0, 0, 0, 1};

	return result;
}

float4x4 float4x4::Scale(float3 scale)
{
	float4x4 result = float4x4::Identity();

	result.m00 = scale.x;
	result.m11 = scale.y;
	result.m22 = scale.z;

	return result;
}

float4x4 float4x4::Identity()
{
	float4x4 result;

	result.m00 = 1;
	result.m11 = 1;
	result.m22 = 1;
	result.m33 = 1;

	return result;
}

float4x4 float4x4::Perspective(float fovy, float aspect, float near, float far)
{
	fovy *= (float)M_PI / 360.0f; // FOVy/2
	float f = cos(fovy) / sin(fovy);

	float4x4 view2proj;

	view2proj.row0 = float4{f / aspect, 0, 0, 0};
	view2proj.row1 = float4{0, f, 0, 0};
	view2proj.row2 = float4{0, 0, (far + near) / (near - far), 2 * far * near / (near - far)};
	view2proj.row3 = float4{0, 0, +1, 0};

	return view2proj;
}

float4x4 float4x4::LookAt(float3 eye, float3 target, float3 up)
{
	float3 f = (target - eye).Normalized();
	float3 u = up.Normalized();
	float3 s = float3::Cross(f, u).Normalized();
	u = float3::Cross(s, f);

	float4x4 world2view;

	// from https://stackoverflow.com/questions/19740463/lookat-function-im-going-crazy but transposed
	world2view.row0 = float4{  s.x,  s.y,  s.z, -float3::Dot(s, eye)};
	world2view.row1 = float4{  u.x,  u.y,  u.z, -float3::Dot(u, eye)};
	world2view.row2 = float4{  f.x,  f.y,  f.z, -float3::Dot(f, eye)}; 
	world2view.row3 = float4{0, 0, 0, 1};

	return world2view;
}
