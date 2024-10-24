#include "meshBuilder.h"
#include "mesh.h"
#define _USE_MATH_DEFINES
#include "math.h"

static void RecalculateNormals(Mesh& mesh) 
{
	for (Vertex& v : mesh.vertices)
	{
		v.normal = float3(0, 0, 0);
	}

	for (int i = 0; i < mesh.indices.size(); ++i)
	{
		int3 index = mesh.indices[i];
		float3 a = mesh.vertices[index.a].position;
		float3 b = mesh.vertices[index.b].position;
		float3 c = mesh.vertices[index.c].position;

		float3 normal = float3::Cross(c - a, b - a);

		mesh.vertices[index.a].normal += normal;
		mesh.vertices[index.b].normal += normal;
		mesh.vertices[index.c].normal += normal;
	}

	for (Vertex& v : mesh.vertices)
	{
		v.normal.Normalize();
	}
}

Mesh MeshBuilder::BuildTriangle()
{
	Mesh m;

	m.vertices.push_back(Vertex(float3(0, 0, 0), float3(0, 0, 1)));
	m.vertices.push_back(Vertex(float3(0, 2, 0), float3(0, 0, 1)));
	m.vertices.push_back(Vertex(float3(2, 0, 0), float3(0, 0, 1)));

	m.indices.push_back(int3(0, 1, 2));

	return m;
}

Mesh MeshBuilder::BuildCone()
{
	Mesh m;

	constexpr int numBaseVertices = 12;
	constexpr float radius = 1.0f;
	constexpr float height = 1.0f;

	for (int i = 0; i < numBaseVertices; ++i)
	{
		float angle = (float)(2 * M_PI * i / numBaseVertices);
		float x = radius * cos(angle);
		float y = radius * sin(angle);
		m.vertices.push_back(Vertex(float3(x, 0, y), float3(x, 0, y)));
	}

	m.vertices.push_back(Vertex(float3(0, 0, 0), float3(0, 0, -1))); // bottom center
	m.vertices.push_back(Vertex(float3(0, height, 0), float3(0, 0, 1))); // top center

	for (int i = 0; i < numBaseVertices; ++i)
	{
		int next = (i + 1) % numBaseVertices;
		m.indices.push_back(int3(i, numBaseVertices + 1, next)); // top tri
		m.indices.push_back(int3(i, next, numBaseVertices)); // bottom tri
	}

	return m;
}

// adapted from https://gist.github.com/Pikachuxxxx/5c4c490a7d7679824e0e18af42918efc
static void GenerateSphereSmooth(std::vector<Vertex>& vertices, std::vector<int>& indices,
	int radius, int latitudes, int longitudes)
{
	float lengthInv = 1.0f / radius;

	constexpr float pi = (float)M_PI;

	float deltaLatitude = pi / latitudes;
	float deltaLongitude = 2 * pi / longitudes;
	float latitudeAngle;
	float longitudeAngle;

	// Compute all vertices first except normals
	for (int i = 0; i <= latitudes; ++i)
	{
		latitudeAngle = pi / 2 - i * deltaLatitude; /* Starting -pi/2 to pi/2 */
		float xy = radius * cosf(latitudeAngle);    /* r * cos(phi) */
		float z = radius * sinf(latitudeAngle);     /* r * sin(phi )*/

		/*
		 * We add (latitudes + 1) vertices per longitude because of equator,
		 * the North pole and South pole are not counted here, as they overlap.
		 * The first and last vertices have same position and normal, but
		 * different tex coords.
		 */
		for (int j = 0; j <= longitudes; ++j)
		{
			longitudeAngle = j * deltaLongitude;

			Vertex vertex;
			vertex.position.x = xy * cosf(longitudeAngle);   /* x = r * cos(phi) * cos(theta)  */
			vertex.position.y = xy * sinf(longitudeAngle);   /* y = r * cos(phi) * sin(theta) */
			vertex.position.z = z;                           	/* z = r * sin(phi) */

			// normalized vertex normal
			vertex.normal.x = vertex.position.x * lengthInv;
			vertex.normal.y = vertex.position.y * lengthInv;
			vertex.normal.z = vertex.position.z * lengthInv;

			// Texture coordinates
			vertex.u = (float)j/longitudes;
			vertex.v = (float)i/latitudes;

			vertices.push_back(vertex);
		}
	}

	/*
	 *  Indices
	 *  k1--k1+1
	 *  |  / |
	 *  | /  |
	 *  k2--k2+1
	 */
	unsigned int k1, k2;
	for (int i = 0; i < latitudes; ++i)
	{
		k1 = i * (longitudes + 1);
		k2 = k1 + longitudes + 1;
		// 2 Triangles per latitude block excluding the first and last longitudes blocks
		for (int j = 0; j < longitudes; ++j, ++k1, ++k2)
		{
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k1 + 1);
				indices.push_back(k2);
			}

			if (i != (latitudes - 1))
			{
				indices.push_back(k2);
				indices.push_back(k1 + 1);
				indices.push_back(k2 + 1);
			}
		}
	}
}

Mesh MeshBuilder::BuildUnitSphere(int subdivisions)
{
	Mesh m;

	std::vector<int> indices;
	GenerateSphereSmooth(m.vertices, indices, 1, subdivisions, subdivisions);

	for (int i = 0; i < indices.size(); i += 3)
	{
		m.indices.push_back(int3(indices[i], indices[i + 1], indices[i + 2]));
	}

	return m;
}

Mesh MeshBuilder::BuildTorus()
{
	Mesh m;

	constexpr int numRings = 16;
	constexpr int verticesPerRing = 12;

	for (int i = 0; i < numRings; ++i)
	{
		float theta = (float)(2 * M_PI * i / numRings);
		float x = cos(theta);
		float z = sin(theta);

		for (int j = 0; j < verticesPerRing; ++j)
		{
			float phi = (float)(2 * M_PI * j / verticesPerRing);
			float y = cos(phi);
			float r = 2.0f + 1.0f * sin(phi);

			m.vertices.push_back(Vertex(float3(x * r, y, z * r), float3(x, 0, z)));
		}
	}
	
	for (int i = 0; i < numRings; ++i)
	{
		for (int j = 0; j < verticesPerRing; ++j)
		{
			int nextI = (i + 1) % numRings;
			int nextJ = (j + 1) % verticesPerRing;

			m.indices.push_back(int3(i * verticesPerRing + j, i * verticesPerRing + nextJ, nextI * verticesPerRing + j));
			m.indices.push_back(int3(i * verticesPerRing + nextJ, nextI * verticesPerRing + nextJ, nextI * verticesPerRing + j));
		}
	}

	RecalculateNormals(m);

	return m;
}

Mesh MeshBuilder::BuildCube() 
{
	static constexpr float vertices[] = 
	{
		// positions			  // normals			// uvs			// colors (all white)
		-0.5f, -0.5f, -0.5f,	  0.0f,  0.0f, -1.0f,	0.0f, 0.0f,		1.0f, 1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,	  0.0f,  0.0f, -1.0f,	1.0f, 0.0f,		1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,	  0.0f,  0.0f, -1.0f,	1.0f, 1.0f,		1.0f, 1.0f, 1.0f,
		
		 0.5f,  0.5f, -0.5f,	  0.0f,  0.0f, -1.0f,	1.0f, 1.0f,		1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,	  0.0f,  0.0f, -1.0f,	0.0f, 1.0f,		1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	  0.0f,  0.0f, -1.0f,	0.0f, 0.0f,		1.0f, 1.0f, 1.0f,


		-0.5f, -0.5f,  0.5f,	  0.0f,  0.0f, 1.0f,	0.0f, 0.0f,		1.0f, 1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,	  0.0f,  0.0f, 1.0f,	1.0f, 0.0f,		1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,	  0.0f,  0.0f, 1.0f,	1.0f, 1.0f,		1.0f, 1.0f, 1.0f,

		 0.5f,  0.5f,  0.5f,	  0.0f,  0.0f, 1.0f,	1.0f, 1.0f,		1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,	  0.0f,  0.0f, 1.0f,	0.0f, 1.0f,		1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,	  0.0f,  0.0f, 1.0f,	0.0f, 0.0f,		1.0f, 1.0f, 1.0f,


		-0.5f,  0.5f,  0.5f,	 -1.0f,  0.0f,  0.0f,	1.0f, 0.0f,		1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,	 -1.0f,  0.0f,  0.0f,	1.0f, 1.0f,		1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	 -1.0f,  0.0f,  0.0f,	0.0f, 1.0f,		1.0f, 1.0f, 1.0f,

		-0.5f, -0.5f, -0.5f,	 -1.0f,  0.0f,  0.0f,	0.0f, 1.0f,		1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,	 -1.0f,  0.0f,  0.0f,	0.0f, 0.0f,		1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,	 -1.0f,  0.0f,  0.0f,	1.0f, 0.0f,		1.0f, 1.0f, 1.0f,


		 0.5f,  0.5f,  0.5f,	  1.0f,  0.0f,  0.0f,	1.0f, 0.0f,		1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,	  1.0f,  0.0f,  0.0f,	1.0f, 1.0f,		1.0f, 1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,	  1.0f,  0.0f,  0.0f,	0.0f, 1.0f,		1.0f, 1.0f, 1.0f,

		 0.5f, -0.5f, -0.5f,	  1.0f,  0.0f,  0.0f,	0.0f, 1.0f,		1.0f, 1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,	  1.0f,  0.0f,  0.0f,	0.0f, 0.0f,		1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,	  1.0f,  0.0f,  0.0f,	1.0f, 0.0f,		1.0f, 1.0f, 1.0f,


		-0.5f, -0.5f, -0.5f,	  0.0f, -1.0f,  0.0f,	0.0f, 1.0f,		1.0f, 1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,	  0.0f, -1.0f,  0.0f,	1.0f, 1.0f,		1.0f, 1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,	  0.0f, -1.0f,  0.0f,	1.0f, 0.0f,		1.0f, 1.0f, 1.0f,

		 0.5f, -0.5f,  0.5f,	  0.0f, -1.0f,  0.0f,	1.0f, 0.0f,		1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,	  0.0f, -1.0f,  0.0f,	0.0f, 0.0f,		1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	  0.0f, -1.0f,  0.0f,	0.0f, 1.0f,		1.0f, 1.0f, 1.0f,


		-0.5f,  0.5f, -0.5f,	  0.0f,  1.0f,  0.0f,	0.0f, 1.0f,		1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,	  0.0f,  1.0f,  0.0f,	1.0f, 1.0f,		1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,	  0.0f,  1.0f,  0.0f,	1.0f, 0.0f,		1.0f, 1.0f, 1.0f,

		 0.5f,  0.5f,  0.5f,	  0.0f,  1.0f,  0.0f,	1.0f, 0.0f,		1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,	  0.0f,  1.0f,  0.0f,	0.0f, 0.0f,		1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,	  0.0f,  1.0f,  0.0f,	0.0f, 1.0f,		1.0f, 1.0f, 1.0f,
	};
	
	static constexpr unsigned int indices[] = 
	{
		0,2,1,
		3,5,4,
		6,8,7,
		9,11,10,
		12,14,13,
		15,17,16,

		18,19,20,
		21,22, 23,
		
		24,26,25,
		27,29,28,

		30,31,32,
		33,34,35,
	};

	Mesh m;
	for (int i = 0; i < sizeof(vertices) / sizeof(float); i += 11)
	{
		Vertex v;
		v.position.x = vertices[i + 0];
		v.position.y = vertices[i + 1];
		v.position.z = vertices[i + 2];

		v.normal.x = vertices[i + 3];
		v.normal.y = vertices[i + 4];
		v.normal.z = vertices[i + 5];

		v.u = vertices[i + 6];
		v.v = vertices[i + 7];

		v.color.x = vertices[i + 8];
		v.color.y = vertices[i + 9];
		v.color.z = vertices[i + 10];

		m.vertices.push_back(v);
	}

	for (int i = 0; i < sizeof(indices) / sizeof(int); i += 3) 
	{
		m.indices.push_back(int3(indices[i+0], indices[i+1], indices[i+2]));
	}

	return m;
}