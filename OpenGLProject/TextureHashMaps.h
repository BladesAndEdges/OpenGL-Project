#pragma once

#include <string>
#include <unordered_map>
#include "Texture.h"

class TextureHashMaps
{
private:

	std::unordered_map<std::string, Texture> m_textures;

public:

	TextureHashMaps();

	void addTexture(const std::string& path, Texture&& texture);
	const Texture* getTexture(const std::string& path) const;


};

