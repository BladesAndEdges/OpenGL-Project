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
void Camera::getFrustumCornersInWorldSpace(glm::vec3 * frustumCorners) const
{
	computeFrustumPlaneCornersInWorldSpace(m_near, frustumCorners);
	computeFrustumPlaneCornersInWorldSpace(m_far, frustumCorners + 4);
}

// --------------------------------------------------------------------------------
glm::vec3 Camera::getWorldSpaceFrustumCenter() const
{
	glm::vec3 frustumCorners[8];
	computeFrustumPlaneCornersInWorldSpace(m_near, frustumCorners);
	computeFrustumPlaneCornersInWorldSpace(m_far, frustumCorners + 4);

	glm::vec3 min = frustumCorners[0];
	glm::vec3 max = min;

	for (uint32_t corner = 1; corner < 8; corner++)
	{
		min = glm::min(min, frustumCorners[corner]);
		max = glm::max(max, frustumCorners[corner]);
	}

	return ((min + max) / 2.0f);
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

// --------------------------------------------------------------------------------
void Camera::computeFrustumPlaneCornersInWorldSpace(float planeDistance, glm::vec3 * planeCornersStart) const
{
	// Compute width and height of the plane in view space
	float halfWidth = planeDistance * tanf(glm::radians(m_fov) / 2.0f);
	float halfHeight = halfWidth / (m_width / m_height);

	const glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), m_worldPosition);

	planeCornersStart[0] = glm::vec3(translationMatrix * glm::vec4(m_worldOrientation * glm::vec3(-halfWidth, -halfHeight, planeDistance), 1.0f));
	planeCornersStart[1] = glm::vec3(translationMatrix * glm::vec4(m_worldOrientation * glm::vec3(halfWidth, -halfHeight, planeDistance), 1.0f));
	planeCornersStart[2] = glm::vec3(translationMatrix * glm::vec4(m_worldOrientation * glm::vec3(halfWidth, halfHeight, planeDistance), 1.0f));
	planeCornersStart[3] = glm::vec3(translationMatrix * glm::vec4(m_worldOrientation * glm::vec3(-halfWidth, halfHeight, planeDistance), 1.0f));
}
