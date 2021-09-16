#include "Camera.h"

Camera::Camera() : m_worldPosition(glm::vec3(0.0f, 0.0f, 0.0f)), 
					m_worldOrientation(glm::mat3(1.0f))
{
}

glm::mat4 Camera::createViewMatrix() const
{
	glm::mat4 viewMatrix;

	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -m_worldPosition);

	viewMatrix = translationMatrix;

	return viewMatrix;
}

glm::vec3 Camera::getWorldPosition() const
{
	return m_worldPosition;
}

glm::mat3 Camera::getWorldOrientation() const
{
	return m_worldOrientation;
}

void Camera::setCameraWorldPosition(const glm::vec3 & newWorldPosition)
{
	m_worldPosition = newWorldPosition;
}
