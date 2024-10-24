#pragma once

class Buffer;
class Mesh;
struct DirectionalLight;
struct Transform;
struct PointLight;
struct SpotLight;
struct Camera;

#include <vector>

namespace Renderer 
{
	void DrawMesh(
		Buffer& buffer, 
		const Mesh& mesh, 
		const Transform& transform, 
		const Camera& camera, 
		const DirectionalLight& directionalLight, 
		const std::vector<PointLight>& pointLights, 
		const SpotLight& spotLight);
	float ToCanonicalSpace(int value, float limit);
	int ToPixelSpace(float value, int limit);
}