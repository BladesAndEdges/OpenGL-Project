#include "MaterialReader.h"

#include <iostream>
#include <fstream>
#include "PerMaterialUniformData.h"

// --------------------------------------------------------------------------------
MaterialReader::MaterialReader(const std::string& rootMeshDirectory, const std::string& cacheSubFolder) : m_rootMeshDirectory(rootMeshDirectory), 
																											m_cacheSubFolder(cacheSubFolder)
																							
{
	loadTexture(R"(Meshes\sponza\textures\dummy_ddn.png)", m_cacheSubFolder, TextureTarget::Texture2D, TextureWrapMode::Repeat, TextureFilterMode::Point); // default normal map
	loadTexture(R"(Meshes\sponza\textures\dummy_mask.png)", m_cacheSubFolder, TextureTarget::Texture2D, TextureWrapMode::Repeat, TextureFilterMode::Point); // default mask texture
	loadTexture(R"(Meshes\sponza\textures\black8x8.png)", m_cacheSubFolder, TextureTarget::Texture2D, TextureWrapMode::Repeat, TextureFilterMode::Point); // default texture for missing textures
}

// --------------------------------------------------------------------------------
MaterialReader::~MaterialReader()
{
	m_Materials.clear();
	m_textures.clear();
}

// --------------------------------------------------------------------------------
void MaterialReader::parseMaterialFile(const std::string & fileName)
{
	std::ifstream ifs(fileName);

	if (!ifs.is_open())
	{
		throw std::exception("Could not read Material Data!");
	}
	// Exception will be thrown here, because do not have GIT LFS, and 
	// could not commit the Emerald Square file from Nvidia, even in compiled format, 

	Material currentMaterial;
	PerMaterialUniformData perMaterialUniforms;
	std::string materialName;
	std::string str;

	bool firstMaterial = true;

	while (ifs >> str)
	{
		if (str == "newmtl")
		{
			if (!firstMaterial)
			{
				completeMaterial(currentMaterial);
				const std::string uniformBufferName = materialName + std::string(" UniformBuffer");
				currentMaterial.m_uniformBuffer = new UniformBuffer(7u, sizeof(PerMaterialUniformData), &perMaterialUniforms, uniformBufferName.c_str());
				m_Materials.insert({ materialName, currentMaterial });
			}

			currentMaterial.m_baseColourTexture = nullptr;
			currentMaterial.m_metallicTexture = nullptr;
			currentMaterial.m_normalMapTexture = nullptr;
			currentMaterial.m_maskTexture = nullptr;
			currentMaterial.m_uniformBuffer = nullptr;
			ZeroMemory(&perMaterialUniforms, sizeof(PerMaterialUniformData));

			ifs >> materialName;

			firstMaterial = false;
		}

		if (str == "map_Kd")
		{
			std::string texturePath = processTexturePath(ifs);

			const std::string finalPath = m_rootMeshDirectory + texturePath;

			if (m_textures.find(finalPath) == m_textures.end())
			{
				loadTexture(finalPath.c_str(), m_cacheSubFolder, TextureTarget::Texture2D, TextureWrapMode::Repeat, TextureFilterMode::Trilinear);
				currentMaterial.m_baseColourTexture = &m_textures.at(finalPath);
				
				std::cout << "Loaded BaseColour Texture: " << texturePath << std::endl;
			}
			else
			{
				currentMaterial.m_baseColourTexture = &m_textures.at(finalPath);
			}
		}

		if (str == "map_Ks")
		{
			std::string texturePath = processTexturePath(ifs);

			const std::string finalPath = m_rootMeshDirectory + texturePath;

			if (m_textures.find(finalPath) == m_textures.end())
			{
				loadTexture(finalPath.c_str(), m_cacheSubFolder, TextureTarget::Texture2D, TextureWrapMode::Repeat, TextureFilterMode::Trilinear);
				currentMaterial.m_baseColourTexture = &m_textures.at(finalPath);

				std::cout << "Loaded Metalness Texture: " << texturePath << std::endl;
			}
			else
			{
				currentMaterial.m_metallicTexture = &m_textures.at(finalPath);
			}
		}

		if ((str == "map_normal") || (str == "map_Bump"))
		{
			std::string texturePath = processTexturePath(ifs);

			const std::string finalPath = m_rootMeshDirectory + texturePath;

			if (m_textures.find(finalPath) == m_textures.end())
			{
				loadTexture(finalPath.c_str(), m_cacheSubFolder, TextureTarget::Texture2D, TextureWrapMode::Repeat, TextureFilterMode::Trilinear);
				currentMaterial.m_normalMapTexture = &m_textures.at(finalPath);
				
				std::cout << "Loaded Normal Texture: " << texturePath << std::endl;
			}
			else
			{
				currentMaterial.m_normalMapTexture = &m_textures.at(finalPath);
			}
		}

		if (str == "map_d")
		{
			std::string texturePath = processTexturePath(ifs);

			const std::string finalPath = m_rootMeshDirectory + texturePath;

			if (m_textures.find(finalPath) == m_textures.end())
			{
				loadTexture(finalPath.c_str(), m_cacheSubFolder, TextureTarget::Texture2D, TextureWrapMode::Repeat, TextureFilterMode::Trilinear);
				currentMaterial.m_maskTexture = &m_textures.at(finalPath);

				std::cout << "Loaded Mask Texture: " << texturePath << std::endl;
			}
			else
			{
				currentMaterial.m_maskTexture = &m_textures.at(finalPath);
			}
		}

		if (str == "Ns")
		{
			ifs >> perMaterialUniforms.m_specularHighlight;
		}
	}

	completeMaterial(currentMaterial);

	// !!!!!! THE BLOCK INDEX IS HARD-CODED!
	const std::string uniformBufferName = materialName + std::string(" UniformBuffer");
	currentMaterial.m_uniformBuffer = new UniformBuffer(7u, sizeof(PerMaterialUniformData), &perMaterialUniforms, uniformBufferName.c_str());
	m_Materials.insert({ materialName, currentMaterial });
}

// --------------------------------------------------------------------------------
const Material * MaterialReader::getMaterial(const std::string & materialName) const
{
	return &m_Materials.at(materialName);
}

// --------------------------------------------------------------------------------
void MaterialReader::loadTexture(const char * path, const std::string& cacheSubFolder, TextureTarget target, TextureWrapMode wrapMode, TextureFilterMode filterMode)
{
	Texture texture(path, cacheSubFolder, target, wrapMode, filterMode);
	m_textures.insert({ path, std::move(texture) });
}

// --------------------------------------------------------------------------------
void MaterialReader::provideNormalMapTexture(Material& material)
{
	const std::string path = R"(Meshes\sponza\textures\dummy_ddn.png)";
	material.m_normalMapTexture = &m_textures.at(path);
}

// --------------------------------------------------------------------------------
void MaterialReader::provideMaskTexture(Material& material)
{
	const std::string path = R"(Meshes\sponza\textures\dummy_mask.png)";
	material.m_maskTexture = &m_textures.at(path);
}

// --------------------------------------------------------------------------------
void MaterialReader::provideBlackTexture(Material & material, uint32_t id)
{
	assert(id >= 0);
	assert(id <= 2);

	const std::string defaultBlackTexturePath = R"(Meshes\sponza\textures\black8x8.png)";

	switch (id)
	{
	case 0:
		material.m_baseColourTexture = &m_textures.at(defaultBlackTexturePath);
		break;

	case 1: 
		material.m_metallicTexture = &m_textures.at(defaultBlackTexturePath);
		break;

	default:
		std::exception("Non-existant texture reference id used");
		break;
	}
}

// --------------------------------------------------------------------------------
void MaterialReader::completeMaterial(Material & material)
{
	// Provide a black texture for missing light components
	if (material.m_baseColourTexture == nullptr) { provideBlackTexture(material, 0); };
	if (material.m_metallicTexture == nullptr) { provideBlackTexture(material, 1); };

	if (material.m_normalMapTexture == nullptr) { provideNormalMapTexture(material); };
	if (material.m_maskTexture == nullptr) { provideMaskTexture(material); };
}

// --------------------------------------------------------------------------------
std::string processTexturePath(std::ifstream & ifs)
{
	const char escapeChar = '\\';

	std::string texturePath;
	char currentChar;
	while (ifs >> currentChar)
	{
		if (currentChar == escapeChar)
		{
			const char nextChar = (char)ifs.peek();
			handleEscapeSequence(ifs, nextChar);

			texturePath.push_back(currentChar);
		}
		else
		{
			texturePath.push_back(currentChar);
		}

		if (ifs.peek() == '\n')
		{
			break;
		}
	}

	return texturePath;
}

// --------------------------------------------------------------------------------
void handleEscapeSequence(std::ifstream& ifs, const char secondCharInEscapeSequence)
{
	if (secondCharInEscapeSequence == '\\')
	{
		char redundantBackSlash;
		ifs >> redundantBackSlash;
	}
}
