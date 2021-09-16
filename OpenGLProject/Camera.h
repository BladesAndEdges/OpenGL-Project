#pragma once

#include<glm.hpp>
#include<gtc/matrix_transform.hpp>

class Camera
{

private:

	glm::vec3 m_worldPosition;
	glm::mat3 m_worldOrientation;

public:

	Camera();

	glm::mat4 createViewMatrix() const;

	glm::vec3 getWorldPosition() const;
	glm::mat3 getWorldOrientation() const;

	void setCameraWorldPosition(const glm::vec3& newWorldPosition);

};

