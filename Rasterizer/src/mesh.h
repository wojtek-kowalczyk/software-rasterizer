#pragma once

#include <vector>

#include "math/float3.h"
#include "math/int3.h"
#include "math/float4x4.h"

struct Camera
{
	float3 position;
	float3 target;
};

struct Transform
{
	float3 translation;
	float3 rotation;
	float3 scale;

	float4x4 GetModelMatrix() const;
};

struct Vertex
{
	Vertex() : position(0, 0, 0), normal(0, 0, 0), u(0), v(0) {}
	Vertex(float3 position, float3 normal) : position(position), normal(normal), u(0), v(0) {}
	Vertex(float3 position, float3 normal, float3 color) : position(position), normal(normal), color(color), u(0), v(0) {}

	float3 position;
	float3 normal;
	float3 color = float3(1,1,1);
	float u;
	float v;
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<int3> indices; // int3 = triangle
	void SetColor(float3 color);
	Mesh Transformed(const Transform& transform, const Camera& camera, float aspectRatio) const;
	static void TransformVertex(Vertex& v, const Transform& transform, const Camera& camera, float aspectRatio);
	class Buffer* texture = nullptr;
};