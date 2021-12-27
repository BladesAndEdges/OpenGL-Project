#pragma once

#pragma warning(push)
#pragma warning(disable:4201)   
#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#pragma warning(pop)

// --------------------------------------------------------------------------------
class Camera
{

public:

	Camera();
	Camera(const glm::vec3& cameraStartingPosition);
	Camera(const glm::vec3& cameraStartingPosition, float cameraWidth, float cameraHeight, float cameraNearPlane, float cameraFarPlane);

	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;

	glm::mat4 createViewMatrix() const;
	glm::mat4 createProjectionMatrix() const;

	glm::vec3 getWorldPosition() const;
	glm::mat3 getWorldOrientation() const;

	void setCameraWorldPosition(const glm::vec3& newWorldPosition);
	void setCameraWorldOrientation(const glm::mat3& newWorldOrientation);

private:

	glm::vec3 m_worldPosition;
	glm::mat3 m_worldOrientation;

	// --------------------------------------------------------------------------------
	// For Orthographic projection
	float m_cameraWidth;
	float m_cameraHeight;
	float m_nearPlane;
	float m_farPlane;

};

