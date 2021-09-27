#pragma once

#include <string>
#include <unordered_map>
#include "Texture.h"

class TextureHashMaps
{
private:

	std::unordered_map<unsigned int, Texture> m_ambientTextures;
	std::unordered_map<unsigned int, Texture> m_diffuseTextures;
	std::unordered_map<unsigned int, Texture> m_specularTextures;

public:

	TextureHashMaps();

	void addAmbientTexture(unsigned int textureId, const Texture& texture);
	void addDiffuseTexture(unsigned int textureId, const Texture& texture);
	void addSpecularTexture(unsigned int textureId, const Texture& texture);

	const Texture* getAmbientTexture(unsigned int id) const;
	const Texture* getDiffuseTexture(unsigned int id) const;
	const Texture* getSpecularTexture(unsigned int id) const;
};

