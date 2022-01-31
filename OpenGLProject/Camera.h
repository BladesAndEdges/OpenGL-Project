#pragma once

#pragma warning(push)
#pragma warning(disable:4201)   
#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#pragma warning(pop)

enum class ProjectionType
{
	ORTHOGRAPHIC, 
	PERSPECTIVE
};

// --------------------------------------------------------------------------------
class Camera
{

public:

	static Camera orthographic(const glm::vec3& worldPosition, float width, float height,
												float near, float far);

	static Camera perspective(const glm::vec3& worldPosition, float width, float height, 
														float near, float far, float fov);

	glm::mat4 createViewMatrix() const;
	glm::mat4 createProjectionMatrix() const;

	void computeFrustumPlaneCornersInWorldSpace(float planeDistance, glm::vec3* frustumCorners) const;


	glm::vec3 getWorldPosition() const;
	glm::mat3 getWorldOrientation() const;
	float getNearPlane() const;
	float getFarPlane() const;

	void setCameraWorldPosition(const glm::vec3& newWorldPosition);
	void setCameraWorldOrientation(const glm::mat3& newWorldOrientation);

	void setCameraWidth(float width);
	void setCameraHeight(float height);

private:

	Camera(ProjectionType projectionType, const glm::vec3& worldPosition, float width, float height, float near, float far, float fov);

	void computeFrustumPlaneCornersInWorldSpace(float planeDistance, glm::vec3* planeCornersStart) const;

	glm::vec3 m_worldPosition;
	glm::mat3 m_worldOrientation;

	ProjectionType m_projectionType;

	float m_width;
	float m_height;
	float m_near;
	float m_far;
	float m_fov;
};

