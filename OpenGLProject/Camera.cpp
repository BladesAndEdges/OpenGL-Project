#include "Camera.h"

// --------------------------------------------------------------------------------
const glm::vec3 magnificationVec = glm::vec3(257.0f, 488.0f, -238.0f);
const glm::vec3 minificationVec = glm::vec3(228.0f, 488.0f, 141.0f);
const glm::vec3 cubePositonVec = glm::vec3(0.0f, 0.0f, -8.0f);
const glm::vec3 cameraNormTest = glm::vec3(-950.0f, 170.0f, -58.0f);

// --------------------------------------------------------------------------------
Camera Camera::orthographic(const glm::vec3 & worldPosition, float width, float height, float near, float far)
{
	return Camera(ProjectionType::ORTHOGRAPHIC, worldPosition, width, height, near, far, 0.0f);
}

// --------------------------------------------------------------------------------
Camera Camera::perspective(const glm::vec3 & worldPosition, float width, float height, float near, float far, float fov)
{
	return Camera(ProjectionType::PERSPECTIVE, worldPosition, width, height, near, far, fov);
}

// --------------------------------------------------------------------------------
glm::mat4 Camera::createViewMatrix() const
{
	const glm::mat4 viewTranslationComponent = glm::translate(glm::mat4(1.0f), -m_worldPosition);
	const glm::mat4 viewRotationComponent = glm::mat4(glm::inverse(m_worldOrientation));

	return viewRotationComponent * viewTranslationComponent;
}

// --------------------------------------------------------------------------------
glm::mat4 Camera::createProjectionMatrix() const
{
	glm::mat4 projectionMatrix;

	if (m_projectionType == ProjectionType::PERSPECTIVE)
	{
		projectionMatrix = glm::perspective(glm::radians(m_fov), m_width / m_height, m_near, m_far);
	}
	else
	{
		assert(m_projectionType == ProjectionType::ORTHOGRAPHIC);

		const float halfWidth = m_width / 2.0f;
		const float halfHeight = m_height / 2.0f;

		const float left = -halfWidth;
		const float right = halfWidth;
		const float bottom = -halfHeight;
		const float top = halfHeight;

		projectionMatrix = glm::ortho(left, right, bottom, top, m_near, m_far);
	}

	return projectionMatrix;
}

// --------------------------------------------------------------------------------
void Camera::computeFrustumPlaneCornersInWorldSpace(float planeDistance, glm::vec3 * worldPlaneFrustumCorners) const
{
	// Compute width and height of the plane in view space
	float halfWidth = planeDistance * tanf(glm::radians(m_fov) / 2.0f);
	float halfHeight = halfWidth / (m_width / m_height);

	const glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), m_worldPosition);

	worldPlaneFrustumCorners[0] = glm::vec3(translationMatrix * glm::vec4(m_worldOrientation * glm::vec3(-halfWidth, -halfHeight, -planeDistance), 1.0f));
	worldPlaneFrustumCorners[1] = glm::vec3(translationMatrix * glm::vec4(m_worldOrientation * glm::vec3(halfWidth, -halfHeight, -planeDistance), 1.0f));
	worldPlaneFrustumCorners[2] = glm::vec3(translationMatrix * glm::vec4(m_worldOrientation * glm::vec3(halfWidth, halfHeight, -planeDistance), 1.0f));
	worldPlaneFrustumCorners[3] = glm::vec3(translationMatrix * glm::vec4(m_worldOrientation * glm::vec3(-halfWidth, halfHeight, -planeDistance), 1.0f));
}

// --------------------------------------------------------------------------------
void Camera::computeFrutsumPlaneCornersInViewSpace(float planeDistance, glm::vec3 * viewSpaceFrustumCorners) const
{
	const float halfWidth = planeDistance * tanf(glm::radians(m_fov) / 2.0f);
	const float halfHeight = halfWidth / (m_width / m_height);

	viewSpaceFrustumCorners[0] = glm::vec3(-halfWidth, -halfHeight, -planeDistance);
	viewSpaceFrustumCorners[1] = glm::vec3(halfWidth, -halfHeight, -planeDistance);
	viewSpaceFrustumCorners[2] = glm::vec3(halfWidth, halfHeight, -planeDistance);
	viewSpaceFrustumCorners[3] = glm::vec3(-halfWidth, halfHeight, -planeDistance);

}

// --------------------------------------------------------------------------------
float Camera::getViewWidth() const
{
	return m_width;
}

// --------------------------------------------------------------------------------
float Camera::getViewHeight() const
{
	return m_height;
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
float Camera::getNearPlane() const
{
	return m_near;
}

// --------------------------------------------------------------------------------
float Camera::getFarPlane() const
{
	return m_far;
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

void Camera::setCameraWidth(float width)
{
	assert(width != 0.0f);
	m_width = width;
}

void Camera::setCameraHeight(float height)
{
	assert(height != 0.0f);
	m_height = height;
}

Camera::Camera(ProjectionType projectionType, const glm::vec3 & worldPosition, float width, float height,
	float near, float far, float fov) : m_projectionType(projectionType),
	m_worldPosition(worldPosition),
	m_width(width),
	m_height(height),
	m_near(near),
	m_far(far),
	m_fov(fov)


{
	glm::mat4 orientation = glm::mat4(1.0f);
	orientation = glm::rotate(orientation, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_worldOrientation = glm::mat3(orientation);
}


