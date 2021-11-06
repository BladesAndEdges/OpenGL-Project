#include "TextureHashMaps.h"

TextureHashMaps::TextureHashMaps()
{
}

void TextureHashMaps::addTexture(const std::string& path, Texture && texture)
{
	m_textures.insert({ path, std::move(texture)});
}

const Texture * TextureHashMaps::getTexture(const std::string& path) const
{
	if (m_textures.find(path) == m_textures.end())
	{
		return nullptr;
	}
	else
	{
		return &m_textures.at(path);
	}
}


