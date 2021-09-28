#pragma once

#include <glm.hpp>
#include <vector>

#include "Texture.h"

struct Material
{
	Material();

	glm::vec3 m_ambientColour;
	glm::vec3 m_diffuseColour;
	glm::vec3 m_specularColour;

	float m_shininess;

	const Texture* m_ambientTexture;
	const Texture* m_diffuseTexture;
	const Texture* m_specularTexture;
};

