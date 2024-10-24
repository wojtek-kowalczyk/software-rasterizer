#include "buffer.h"
#include "renderer.h"
#include "math/float3.h"
#include "math/float4.h"
#include "math/float4x4.h"
#include "meshBuilder.h"
#include "light.h"

#include <cassert>
#include <math.h>
#include <vector>

int main()
{
	Buffer buffer{ 500, 400 };
	buffer.ClearColor(0xff000000); // ARGB

	Camera camera{ float3(0, 2, 7), float3(0, 0, 0) };

	Buffer earthTexture(1,1);
	earthTexture.ReadTGAFromFile("res/earth.tga");
	assert(earthTexture.GetWidth() != 1 && "earth texture didn't load coorrectly");

	Buffer brickTexture(1,1);
	brickTexture.ReadTGAFromFile("res/bricks.tga");
	assert(brickTexture.GetWidth() != 1 && "bricks texture didn't load coorrectly");

	Mesh sphere = MeshBuilder::BuildUnitSphere(100);
	sphere.texture = &earthTexture;
	Transform sphereTransform{ float3(3, 0, 0), float3(0, 0, 0), float3(1, 1, 1) };
	Transform bigSphereTransform{ float3(0, -10, 0), float3(0, 0, 0), float3(1, 1, 1) * 10};
	sphere.SetColor(float3(1, 1, 1));

	Mesh torus = MeshBuilder::BuildTorus();
	Transform torusTransform{ float3(-2, 0, 0), float3(0, 0, 0), float3(1, 1, 1) * 0.5f };
	torus.SetColor(float3(1, 1, 1));

	Mesh cube = MeshBuilder::BuildCube();
	cube.texture = &brickTexture;
	Transform cubeTransform{ float3(-2, 2, 0), float3(-20, 0, 0), float3(1, 1, 1)};
	cube.SetColor(float3(1, 1, 1));

	DirectionalLight directionalLight = { float3(1, 1, 1), float3(1, 1, 1) * 0.2f };
	std::vector<PointLight> pointLights = { 
		PointLight{float3(0, 0, 0), float3(1, 1, 1) * 0.65f},
	};

	SpotLight spotLight = { float3(3, 3, 0), float3(0, -1, 0), float3(1, 1, 0) * 0.35f, cosf(3.14f / 6.0f) };

	Mesh lightSphere = MeshBuilder::BuildUnitSphere();
	Transform lightSphereTransform{ pointLights[0].position, float3(0, 0, 0), float3(0.1f, 0.1f, 0.1f) };
	lightSphere.SetColor(float3(1, 1, 1));

	Renderer::DrawMesh(buffer, sphere, sphereTransform, camera, directionalLight, pointLights, spotLight);
	Renderer::DrawMesh(buffer, sphere, bigSphereTransform, camera, directionalLight, pointLights, spotLight);
	Renderer::DrawMesh(buffer, torus, torusTransform, camera, directionalLight, pointLights, spotLight);
	Renderer::DrawMesh(buffer, lightSphere, lightSphereTransform, camera, directionalLight, pointLights, spotLight);
	Renderer::DrawMesh(buffer, cube, cubeTransform, camera, directionalLight, pointLights, spotLight);

	buffer.SaveTGAFile("image.tga");

	return 0;
}
