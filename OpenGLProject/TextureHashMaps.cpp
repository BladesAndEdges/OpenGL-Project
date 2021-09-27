#include "TextureHashMaps.h"

TextureHashMaps::TextureHashMaps()
{
}

void TextureHashMaps::addAmbientTexture(unsigned int textureId, const Texture & texture)
{
	m_ambientTextures.insert({ textureId, texture });
}

void TextureHashMaps::addDiffuseTexture(unsigned int textureId, const Texture & texture)
{
	m_diffuseTextures.insert({ textureId, texture });
}

void TextureHashMaps::addSpecularTexture(unsigned int textureId, const Texture & texture)
{
	m_specularTextures.insert({ textureId, texture });
}

const Texture * TextureHashMaps::getAmbientTexture(unsigned int id) const
{
	return &m_ambientTextures.at(id);
}

const Texture * TextureHashMaps::getDiffuseTexture(unsigned int id) const
{
	return &m_diffuseTextures.at(id);
}

const Texture * TextureHashMaps::getSpecularTexture(unsigned int id) const
{
	return &m_specularTextures.at(id);
}


