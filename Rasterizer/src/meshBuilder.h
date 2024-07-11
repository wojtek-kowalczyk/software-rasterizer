#pragma once

#include "mesh.h"

namespace MeshBuilder
{
	Mesh BuildTriangle();
	Mesh BuildCone();
	Mesh BuildUnitSphere(int subdivisions = 10);
	Mesh BuildTorus();
	Mesh BuildCube();
}
