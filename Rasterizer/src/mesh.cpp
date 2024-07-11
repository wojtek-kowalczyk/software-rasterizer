#include "mesh.h"

#include <cassert>

float4x4 Transform::GetModelMatrix() const
{
	float3 scaleValue = scale;
	float4x4 scaleMatrix = float4x4::Scale(scaleValue);

	float3 rotationValue = rotation;
	float4x4 rotateX = float4x4::Rotate(rotationValue.x, float3(1, 0, 0));
	float4x4 rotateY = float4x4::Rotate(rotationValue.y, float3(0, 1, 0));
	float4x4 rotateZ = float4x4::Rotate(rotationValue.z, float3(0, 0, 1));

	float3 translationValue = translation;
	float4x4 translationMatrix = float4x4::Translate(translationValue);
	float4x4 objectToWorld = float4x4::Identity();
	objectToWorld = objectToWorld * translationMatrix;
	objectToWorld = objectToWorld * (rotateX * rotateY * rotateZ);
	objectToWorld = objectToWorld * scaleMatrix;

	return objectToWorld;
}

static void DoTransformation(Vertex& v, const Camera& camera, const Transform& transform, float aspectRatio)
{
	float3 normal = v.normal;

	float4x4 objectToWorld = transform.GetModelMatrix();
	float4x4 worldToView = float4x4::LookAt(camera.position, camera.target, float3(0, 1, 0));
	float4x4 viewToProjection = float4x4::Perspective(45.0, aspectRatio, 0.1f, 100.0f);

	float4x4 objectToProjection = viewToProjection * worldToView * objectToWorld;
	float4 transformedVertexPosition = objectToProjection * v.position;
	v.position = float3(transformedVertexPosition) / transformedVertexPosition.w; // Perspective division

	assert(normal.x == v.normal.x && normal.y == v.normal.y && normal.z == v.normal.z); // Make sure we don't mess up the normal
}

void Mesh::SetColor(float3 color)
{
	for (Vertex& v : vertices)
	{
		v.color = color;
	}
}

Mesh Mesh::Transformed(const Transform& transform, const Camera& camera, float aspectRatio) const
{
	Mesh m;
	
	m.indices = this->indices;
	
	for (const Vertex& v : this->vertices)
	{
		Vertex newVertex = v;
		DoTransformation(newVertex, camera, transform, aspectRatio);
		m.vertices.push_back(newVertex);
	}

	return m;
}

void Mesh::TransformVertex(Vertex& v, const Transform& transform, const Camera& camera, float aspectRatio)
{
	DoTransformation(v, camera, transform, aspectRatio);
}
