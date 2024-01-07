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

	MaterialReader(const std::string& rootMeshDirectory, const std::string& cacheSubFolder);
	~MaterialReader();
					
	MaterialReader(const MaterialReader&) = delete;
	MaterialReader& operator=(const MaterialReader&) = delete;

	void parseMaterialFile(const std::string& fileName);

	const Material* getMaterial(const std::string& materialName) const;

private:

	std::string m_rootMeshDirectory;
	std::string m_cacheSubFolder;

	std::unordered_map<std::string, Material> m_Materials;
	std::unordered_map<std::string, Texture> m_textures;
	//TextureHashMaps m_textureHashMaps;

	void loadTexture(const char* path, const std::string& cacheSubFolder,TextureTarget target, TextureWrapMode wrapMode,
		TextureFilterMode filterMode);

	void provideNormalMapTexture(Material& material);
	void provideMaskTexture(Material& material);
	void provideBlackTexture(Material& material, uint32_t id);

	void completeMaterial(Material& material);
};

std::string processTexturePath(std::ifstream& ifs);
void handleEscapeSequence(std::ifstream& ifs, const char secondCharInEscapeSequence);

