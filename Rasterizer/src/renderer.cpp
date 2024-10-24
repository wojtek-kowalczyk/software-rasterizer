#include "renderer.h"

#include "math/float3.h"
#include "light.h"
#include "buffer.h"
#include "mesh.h"

#include <cmath>
#include <cassert>
#include <cstdint>
#include <ios>
#include <iostream>

static float3 GetVertexColor(const Vertex& v, const Transform& transform, const float3& cameraPosition, const DirectionalLight& directionalLight, const std::vector<PointLight>& pointLights, const SpotLight& spotLight)
{
    float4x4 objectToWorld = transform.GetModelMatrix();

    float3 diffuse(0,0,0);
    float3 specular(0,0,0);

    float3 vn = v.normal.Normalized();
    float3 N = objectToWorld * float4{vn.x, vn.y, vn.z, 0.0f}; // 0 ignores translation for normals, scale might be bad. see: https://learnopengl.com/Lighting/Basic-Lighting -> "One Last Thing" section
    N.Normalize();

    // Directional light
    float3 lightDirection = directionalLight.direction.Normalized();
    float intensity = fmax(0.0f, float3::Dot(N, lightDirection));
    diffuse += directionalLight.color * intensity;

    // Point lights
    float3 worldSpaceVertexPosition = objectToWorld * v.position;
    for (const PointLight& pointLight : pointLights)
    {
        // Diffuse
        float3 toLight = (pointLight.position - worldSpaceVertexPosition).Normalized();
		float intensity = fmax(0.0f, float3::Dot(N, toLight));
		diffuse += pointLight.color * intensity;

        // Specular
        float3 reflection = float3::Reflect(-toLight, N);
        float3 toCamera = (cameraPosition - worldSpaceVertexPosition).Normalized();
        float specularIntensity = fmax(0.0f, float3::Dot(reflection, toCamera));
        float value = (float)pow(specularIntensity, 32);
        specular += pointLight.color * value;
	}

    // Spot light
    float3 toSpotlight = (spotLight.position - worldSpaceVertexPosition).Normalized();
    float theta = float3::Dot(toSpotlight, -spotLight.direction.Normalized());
    if (theta > spotLight.angle)
    {
        // Same calc as point light, but limited by the angle
        
        // Diffuse
        float3 toLight = (spotLight.position - worldSpaceVertexPosition).Normalized();
        float intensity = fmax(0.0f, float3::Dot(N, toLight));
        diffuse += spotLight.color * intensity;

        // Specular
        float3 reflection = float3::Reflect(-toLight, N);
        float3 toCamera = (cameraPosition - worldSpaceVertexPosition).Normalized();
        float specularIntensity = fmax(0.0f, float3::Dot(reflection, toCamera));
        float value = (float)pow(specularIntensity, 32);
        specular += spotLight.color * value;
    }

    float3 ambient = float3(0.1f, 0.1f, 0.1f);

    return v.color * (ambient + diffuse + specular).Clamped();
}

static uint32_t SampleTexture(const Buffer* texture, float u, float v)
{
    assert(u > -0.0001f && u < 1.0001f);
    assert(v > -0.0001f && v < 1.0001f);

    const int pixelX = (int)(u * texture->GetWidth());
    const int pixelY = (int)(v * texture->GetHeight());

    return texture->ColorAt(pixelX, pixelY);
}

static void DrawTriangle(Buffer& buffer, Vertex v1, Vertex v2, Vertex v3, Vertex untransformedV1, Vertex untransformedV2, Vertex untransformedV3, 
    const Transform &transform, const float3 &cameraPosition, const DirectionalLight &directionalLight, const std::vector<PointLight> &pointLights,
    const SpotLight &spotLight, const Buffer* texture)
{
    // Optimization 1: if the point is outside the bounding box of the triangle, we can skip it
    float xMin = fmin(v1.position.x, fmin(v2.position.x, v3.position.x));
    float xMax = fmax(v1.position.x, fmax(v2.position.x, v3.position.x));
    float yMin = fmin(v1.position.y, fmin(v2.position.y, v3.position.y));
    float yMax = fmax(v1.position.y, fmax(v2.position.y, v3.position.y));

    // Convert to pixel space
    int xMinPixelSpace = Renderer::ToPixelSpace(xMin, buffer.GetWidth());
    int xMaxPixelSpace = Renderer::ToPixelSpace(xMax, buffer.GetWidth());
    int yMinPixelSpace = Renderer::ToPixelSpace(yMin, buffer.GetHeight());
    int yMaxPixelSpace = Renderer::ToPixelSpace(yMax, buffer.GetHeight());

    // Clamp to buffer size
    xMinPixelSpace = (int)fmax(xMinPixelSpace, 0);
    xMaxPixelSpace = (int)fmin(xMaxPixelSpace, buffer.GetWidth());
    yMinPixelSpace = (int)fmax(yMinPixelSpace, 0);
    yMaxPixelSpace = (int)fmin(yMaxPixelSpace, buffer.GetHeight());

    // Transform the triangle to pixel space from canonical space
    // This allows us to operate on integer values, and does not introduce artifacts caused by floating point precision
    int pv1x = Renderer::ToPixelSpace(v1.position.x, buffer.GetWidth());
    int pv1y = Renderer::ToPixelSpace(v1.position.y, buffer.GetHeight());
    int pv1z = (int)v1.position.z;
    int pv2x = Renderer::ToPixelSpace(v2.position.x, buffer.GetWidth());
    int pv2y = Renderer::ToPixelSpace(v2.position.y, buffer.GetHeight());
    int pv2z = (int)v2.position.z;
    int pv3x = Renderer::ToPixelSpace(v3.position.x, buffer.GetWidth());
    int pv3y = Renderer::ToPixelSpace(v3.position.y, buffer.GetHeight());
    int pv3z = (int)v3.position.z;

    // Optimization 2: compute consts outside the loop (and it will help us with interpolation)
    int dx12 = pv1x - pv2x;
    int dx23 = pv2x - pv3x;
    int dx31 = pv3x - pv1x;
    int dy12 = pv1y - pv2y;
    int dy23 = pv2y - pv3y;
    int dy31 = pv3y - pv1y;
    int dx32 = pv3x - pv2x;
    int dx13 = pv1x - pv3x;
    int dy13 = pv1y - pv3y;

    // Handle filling convention
    bool topleft12 = false;
    bool topleft23 = false;
    bool topleft31 = false;
    if (dy12 < 0 || (dy12 == 0 && dx12 > 0)) { topleft12 = true; }
    if (dy23 < 0 || (dy23 == 0 && dx23 > 0)) { topleft23 = true; }
    if (dy31 < 0 || (dy31 == 0 && dx31 > 0)) { topleft31 = true; }

    // These assertions prevent me from having subpixel polygons
    //assert((topleft12 && topleft23 && topleft31) == false); // 3 can't be true
    //assert((topleft12 || topleft23 || topleft31) == true); // at least 1 must be true

    for (int y = yMinPixelSpace; y < yMaxPixelSpace; y++)
    {
        for (int x = xMinPixelSpace; x < xMaxPixelSpace; x++)
        {
            // Edge functions
            int tmp12 = (dx12 * (y - pv1y) - dy12 * (x - pv1x));
            int tmp23 = (dx23 * (y - pv2y) - dy23 * (x - pv2x));
            int tmp31 = (dx31 * (y - pv3y) - dy31 * (x - pv3x));

            bool belongsToTriangle =
                (topleft12 ? tmp12 >= 0 : tmp12 > 0) &&
                (topleft23 ? tmp23 >= 0 : tmp23 > 0) &&
                (topleft31 ? tmp31 >= 0 : tmp31 > 0);

            if (belongsToTriangle == false)
            {
                continue;
            }

            // Compute barycentric coordinates (l1 + l2 + l3 = 1)
            float lambda1 = (dy23 * (x - pv3x) + dx32 * (y - pv3y)) / (float)(dy23 * dx13 + dx32 * dy13);
            float lambda2 = (dy31 * (x - pv3x) + dx13 * (y - pv3y)) / (float)(dy31 * dx23 + dx13 * dy23);
            float lambda3 = 1.0f - lambda1 - lambda2;
            assert(lambda1 >= -0.00001 && lambda1 <= 1.00001);
            assert(lambda2 >= -0.00001 && lambda2 <= 1.00001);
            assert(lambda3 >= -0.00001 && lambda3 <= 1.00001);

            // interpolate vertex color (when using vertex colors - currently, I'm not)
            // uint8_t red     = static_cast<uint8_t>((v1.color.r * 255.0f * lambda1) + (v2.color.r * 255.0f * lambda2) + (v3.color.r * 255.0f * lambda3));
            // uint8_t green   = static_cast<uint8_t>((v1.color.g * 255.0f * lambda1) + (v2.color.g * 255.0f * lambda2) + (v3.color.g * 255.0f * lambda3));
            // uint8_t blue    = static_cast<uint8_t>((v1.color.b * 255.0f * lambda1) + (v2.color.b * 255.0f * lambda2) + (v3.color.b * 255.0f * lambda3));
            // uint32_t color = (0xff << 24) | (red << 16) | (green << 8) | blue;

            // Compute fragment color
            float3 fragNormal = (untransformedV1.normal * lambda1 + untransformedV2.normal * lambda2 + untransformedV3.normal * lambda3).Normalized();
            float3 fragPosition = (untransformedV1.position * lambda1 + untransformedV2.position * lambda2 + untransformedV3.position * lambda3).Normalized();
            float3 initialFragColor;
            if (texture != nullptr) 
            {
                float u = v1.u * lambda1 + v2.u * lambda2 + v3.u * lambda3;
                float v = v1.v * lambda1 + v2.v * lambda2 + v3.v * lambda3;
                uint32_t sampledColor = SampleTexture(texture, u, v);
                float red   = ((sampledColor & 0x00ff0000) >> 16) / 255.0f;
                float green = ((sampledColor & 0x0000ff00) >> 8)  / 255.0f;
                float blue  = ((sampledColor & 0x000000ff) >> 0)  / 255.0f;
                initialFragColor = float3(red, green, blue);
            }
            else
            {
                initialFragColor = (untransformedV1.color * lambda1 + untransformedV2.color * lambda2 + untransformedV3.color * lambda3).Normalized();
            }
            Vertex fragment {fragPosition, fragNormal, initialFragColor};
            float3 finalFragColor = GetVertexColor(fragment, transform, cameraPosition, directionalLight, pointLights, spotLight);
            uint8_t red     = static_cast<uint8_t>(finalFragColor.r * 255.0f);
            uint8_t green   = static_cast<uint8_t>(finalFragColor.g * 255.0f);
            uint8_t blue    = static_cast<uint8_t>(finalFragColor.b * 255.0f);
            uint32_t color = (0xff << 24) | (red << 16) | (green << 8) | blue;

            // Compute depth based on barycentric coordinates
            float depth = lambda1 * v1.position.z + lambda2 * v2.position.z + lambda3 * v3.position.z;

            if (depth < buffer.DepthAt(x, y))
            {
                buffer.ColorAt(x, y) = color;
                buffer.DepthAt(x, y) = depth;
            }
        }
    }
}

static float3 VisualizeNormal(const float3& normal)
{
    // map components to [0-1] range
    return float3(
        0.5f * (normal.x + 1.0f),
        0.5f * (normal.y + 1.0f),
        0.5f * (normal.z + 1.0f) 
    );
}

void Renderer::DrawMesh(Buffer& buffer, const Mesh& mesh, const Transform& transform, const Camera& camera, const DirectionalLight& directionalLight, const std::vector<PointLight>& pointLights, const SpotLight& spotLight)
{
    for (const int3& triangle : mesh.indices)
    {
        Vertex v1 = mesh.vertices[triangle.a];
        Vertex v2 = mesh.vertices[triangle.b];
        Vertex v3 = mesh.vertices[triangle.c];

        // for shading the vertex needs to not be transformed, but for finding pixel on the screen it needs to be
        Vertex untransformedV1 = v1;
        Vertex untransformedV2 = v2;
        Vertex untransformedV3 = v3;

        Mesh::TransformVertex(v1, transform, camera, buffer.GetAspectRatio());
        Mesh::TransformVertex(v2, transform, camera, buffer.GetAspectRatio());
        Mesh::TransformVertex(v3, transform, camera, buffer.GetAspectRatio());

        DrawTriangle(buffer, v1, v2, v3, untransformedV1, untransformedV2, untransformedV3, transform, 
            camera.position, directionalLight, pointLights, spotLight, mesh.texture);
    }
}

float Renderer::ToCanonicalSpace(int value, float limit)
{
    return (value / (0.5f * limit)) - 1.0f;
}

int Renderer::ToPixelSpace(float value, int limit)
{
    return int((value + 1) * limit * 0.5f);
}

