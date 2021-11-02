#pragma once


#pragma warning(push)
#pragma warning(disable:4201)   
#include<glm.hpp>
#pragma warning(pop)

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
	const Texture* m_normalMapTexture;
	const Texture* m_maskTexture;
};

