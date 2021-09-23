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

	std::vector<Texture> m_textures;
};

