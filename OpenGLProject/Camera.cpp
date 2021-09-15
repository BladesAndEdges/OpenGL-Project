#include "Camera.h"

Camera::Camera() : m_worldPosition(glm::vec3(0.0f, 0.0f, 0.0f)), 
					m_worldOrientation(glm::mat3(1.0f))
{
}

glm::mat3 Camera::createViewMatrix() const
{
	return glm::mat3();
}

glm::vec3 Camera::getWorldPosition() const
{
	return m_worldPosition;
}

glm::mat3 Camera::getWorldOrientation() const
{
	return m_worldOrientation;
}