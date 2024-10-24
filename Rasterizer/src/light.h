#pragma once

#include "math/float3.h"

struct DirectionalLight
{
	float3 direction;
	float3 color;
};

struct PointLight
{
	float3 position;
	float3 color;
};

struct SpotLight
{
	float3 position;
	float3 direction;
	float3 color;
	float angle;
};