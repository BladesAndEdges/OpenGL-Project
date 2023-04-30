#pragma once


#pragma warning(push)
#pragma warning(disable:4201)   
#include<glm.hpp>
#pragma warning(pop)

#include <vector>

#include "Texture.h"
#include "UniformBuffer.h"

// --------------------------------------------------------------------------------
struct Material
{
	Material();
	~Material();

	const Texture* m_ambientTexture;
	const Texture* m_diffuseTexture;
	const Texture* m_specularTexture;
	const Texture* m_normalMapTexture;
	const Texture* m_maskTexture;

	const UniformBuffer* m_uniformBuffer;
};

