#pragma once

#include <string>
#include <unordered_map>

#include "ProfileMarker.h"

#include "Material.h"

#include "TextureHashMaps.h"

// --------------------------------------------------------------------------------
class MaterialReader
{

public:

	MaterialReader();
					
	MaterialReader(const MaterialReader&) = delete;
	MaterialReader& operator=(const MaterialReader&) = delete;

	void parseMaterialFile(const std::string& fileName);

	const Material* getMaterial(const std::string& materialName) const;

private:

	std::unordered_map<std::string, Material> m_Materials;

	TextureHashMaps m_textureHashMaps;

	// Consider making it just loadTexture(const char path, <Texture properties>);
	void loadTexture(const char* name, TextureTarget target, TextureWrapMode wrapMode,
		TextureFilterMode filterMode);

	void provideNormalMapTexture(Material& material);
	void provideMaskTexture(Material& material);
	void provideBlackTexture(Material& material, uint32_t id);

	void completeMaterial(Material& material);
};

