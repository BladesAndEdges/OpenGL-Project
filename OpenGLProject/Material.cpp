#include "Material.h"

// --------------------------------------------------------------------------------
Material::Material() : m_ambientTexture(nullptr), m_diffuseTexture(nullptr), m_specularTexture(nullptr), m_normalMapTexture(nullptr), 
						m_maskTexture(nullptr), m_uniformBuffer(nullptr)
{
	// Create UniformBuffer here
}

// --------------------------------------------------------------------------------
Material::~Material()
{
	//if (m_uniformBuffer != nullptr)
	//{
	//	delete m_uniformBuffer;
	//	m_uniformBuffer;
	//}
}
