#pragma once

#include "math/float3.h"

class Camera
{
public:
	Camera(float3 position);
	void LookAt();

private:
	float3 m_position;
	float3 m_forward;
	float3 m_right;
};