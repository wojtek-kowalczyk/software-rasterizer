#pragma once

#include "float3.h"
#include "float4.h"

struct float4x4 
{
	float4x4();
	float4x4(	float m00, float m01, float m02, float m03,
				float m10, float m11, float m12, float m13,
				float m20, float m21, float m22, float m23,
				float m30, float m31, float m32, float m33);

	union
	{
		struct
		{
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
			float m30, m31, m32, m33;
		};

		struct
		{
			float4 row0;
			float4 row1;
			float4 row2;
			float4 row3;
		};
	};

	float4x4 operator*(const float4x4& other) const;
	float4 operator*(const float4& vector) const;
	float4 operator*(const float3& vector) const;
	float4x4 operator*(float scalar) const;

	void Transpose();

	static float4x4 Translate(float3 translation);
	static float4x4 Rotate(float angle, float3 axis);
	static float4x4 Scale(float3 scale);

	static float4x4 Identity();
	static float4x4 Perspective(float fovy, float aspect, float near, float far);
	static float4x4 LookAt(float3 eye, float3 target, float3 up);
};