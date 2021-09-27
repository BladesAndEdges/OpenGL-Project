#pragma once

#include <string>
#include <unordered_map>
#include "Material.h"

#include "TextureHashMaps.h"

class MaterialReader
{
private:

	std::unordered_map<std::string, Material> m_Materials;

	TextureHashMaps m_textureHashMaps;

public:

	MaterialReader();

	void parseMaterialFile(const std::string& fileName);

	const Material* getMaterial(const std::string& materialName) const;
};

