#include "camera.h"

#include "math/float4x4.h"

Camera::Camera(float3 position)
	:m_position(position), m_forward(0, 0, 1), m_right(1, 0, 0)
{
}

void Camera::LookAt()
{
}
