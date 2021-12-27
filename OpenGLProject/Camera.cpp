#include "Camera.h"
// --------------------------------------------------------------------------------
const glm::vec3 magnificationVec = glm::vec3(257.0f, 488.0f, -238.0f);
const glm::vec3 minificationVec = glm::vec3(228.0f, 488.0f, 141.0f);
const glm::vec3 cubePositonVec = glm::vec3(0.0f, 0.0f, -8.0f);
const glm::vec3 cameraNormTest = glm::vec3(-950.0f, 170.0f, -58.0f);

// --------------------------------------------------------------------------------
Camera::Camera() : m_worldPosition(cameraNormTest)
{
	m_worldOrientation = glm::mat3(1.0f);

	glm::mat4 m = glm::mat4(1.0f);
	m = glm::rotate(m, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_worldOrientation = glm::mat3(m);
}

// --------------------------------------------------------------------------------
Camera::Camera(const glm::vec3 & cameraStartingPosition)
{
	m_worldPosition = cameraStartingPosition;

	glm::mat4 m = glm::mat4(1.0f);
	m = glm::rotate(m, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_worldOrientation = glm::mat3(m);
}

// --------------------------------------------------------------------------------
Camera::Camera(const glm::vec3 & cameraStartingPosition, float cameraWidth, float cameraHeight, float cameraNearPlane, float cameraFarPlane)
	: m_cameraWidth(cameraWidth), m_cameraHeight(cameraHeight), m_nearPlane(cameraNearPlane), m_farPlane(cameraFarPlane)
{
	m_worldPosition = cameraStartingPosition;

	glm::mat4 m = glm::mat4(1.0f);
	m = glm::rotate(m, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_worldOrientation = glm::mat3(m);
}

// --------------------------------------------------------------------------------
glm::mat4 Camera::createViewMatrix() const
glm::mat4 Camera::createProjectionMatrix() const
{
	glm::mat4 projectionMatrix;

	if (m_projectionType == ProjectionType::PERSPECTIVE)
	{
		projectionMatrix = glm::perspective(m_fov, m_width / m_height, m_near, m_far);
	}
	else
	{
		assert(m_projectionType == ProjectionType::ORTHOGRAPHIC);

		const float halfWidth = m_width / 2.0f;
		const float halfHeight = m_height / 2.0f;

		const float left = -halfWidth;
		const float right = halfWidth;
		const float top = -halfHeight;
		const float bottom = halfHeight;

		projectionMatrix = glm::ortho(left, right, bottom, top, m_near, m_far);
	}

	return projectionMatrix;
}

// --------------------------------------------------------------------------------
glm::vec3 Camera::getWorldPosition() const
{
	return m_worldPosition;
}

// --------------------------------------------------------------------------------
glm::mat3 Camera::getWorldOrientation() const
{
	return m_worldOrientation;
}

// --------------------------------------------------------------------------------
void Camera::setCameraWorldPosition(const glm::vec3 & newWorldPosition)
{
	m_worldPosition = newWorldPosition;
}

// --------------------------------------------------------------------------------
void Camera::setCameraWorldOrientation(const glm::mat3 & newWorldOrientation)
{
	m_worldOrientation = newWorldOrientation;
}
