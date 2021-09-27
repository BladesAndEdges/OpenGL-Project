#include "Camera.h"

const glm::vec3 magnificationVec = glm::vec3(257.0f, 488.0f, -238.0f);
const glm::vec3 minificationVec = glm::vec3(228.0f, 488.0f, 141.0f);

Camera::Camera() : m_worldPosition(magnificationVec)
{
	m_worldOrientation = glm::mat3(1.0f);
}

glm::mat4 Camera::createViewMatrix() const
{
	glm::mat4 viewMatrix;

	glm::mat4 viewTranslationComponent = glm::translate(glm::mat4(1.0f), -m_worldPosition);

	glm::mat4 viewRotationComponent = glm::mat4(glm::inverse(m_worldOrientation));

	viewMatrix = viewRotationComponent * viewTranslationComponent;

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

void Camera::setCameraWorldOrientation(const glm::mat3 & newWorldOrientation)
{
	m_worldOrientation = newWorldOrientation;
}
