#pragma once

#include <string>
#include <unordered_map>
#include "Material.h"

#include "TextureHashMaps.h"

class MaterialReader
{

public:

	MaterialReader();

	void parseMaterialFile(const std::string& fileName);

	const Material* getMaterial(const std::string& materialName) const;

private:

	std::unordered_map<std::string, Material> m_Materials;

	TextureHashMaps m_textureHashMaps;

	void provideNormalMapTexture(Material& material);
	void provideMaskTexture(Material& material);
	void provideBlackTexture(Material& material, uint32_t id);

	void completeTexture(Material& material);
};

