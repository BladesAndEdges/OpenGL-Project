#include "MaterialReader.h"

#include <iostream>
#include <fstream>


MaterialReader::MaterialReader()
{
	Texture defaultNormalMap(R"(Meshes\sponza\textures\dummy_ddn.png)", TextureTarget::Texture2D, TextureWrapMode::Repeat,
		TextureFilterMode::Point);

	Texture defaultMaskTexture(R"(Meshes\sponza\textures\dummy_mask.png)", TextureTarget::Texture2D, TextureWrapMode::Repeat,
		TextureFilterMode::Point);

	m_textureHashMaps.addTexture(R"(Meshes\sponza\textures\dummy_ddn.png)", std::move(defaultNormalMap));
	m_textureHashMaps.addTexture(R"(Meshes\sponza\textures\dummy_mask.png)", std::move(defaultMaskTexture));
}

void MaterialReader::parseMaterialFile(const std::string & fileName)
{
	std::ifstream ifs(fileName);

	if (!ifs.is_open())
	{
		throw std::exception("Could not read Material Data!");
	}

	Material currentMaterial;
	std::string materialName;

	std::string str;

	bool firstMaterial = true;

	while (ifs >> str)
	{
		if (str == "newmtl")
		{
			if (!firstMaterial)
			{
				if (currentMaterial.m_normalMapTexture == nullptr) { provideNormalMapTexture(currentMaterial); };
				if (currentMaterial.m_maskTexture == nullptr) { provideMaskTexture(currentMaterial); };
				m_Materials.insert({ materialName, currentMaterial });
			}

			currentMaterial.m_ambientTexture = nullptr;
			currentMaterial.m_diffuseTexture = nullptr;
			currentMaterial.m_specularTexture = nullptr;
			currentMaterial.m_normalMapTexture = nullptr;
			currentMaterial.m_maskTexture = nullptr;

			ifs >> materialName;

			firstMaterial = false;
		}

		if (str == "Ka")
		{
			glm::vec3 ambient;
			ifs >> ambient.x >> ambient.y >> ambient.z;
			currentMaterial.m_ambientColour = ambient;
		}

		if (str == "Kd")
		{
			glm::vec3 diffuse;
			ifs >> diffuse.x >> diffuse.y >> diffuse.z;
			currentMaterial.m_diffuseColour = diffuse;
		}

		if (str == "Ks")
		{
			glm::vec3 specular;
			ifs >> specular.x >> specular.y >> specular.z;
			currentMaterial.m_specularColour = specular;
		}

		if (str == "map_Ka")
		{
			std::string texturePath;
			ifs >> texturePath;

			const std::string path = R"(Meshes\sponza\)";
			const std::string finalPath = path + texturePath;

			if (m_textureHashMaps.getTexture(finalPath) == nullptr)
			{
				Texture texture(finalPath, TextureTarget::Texture2D, TextureWrapMode::Repeat, TextureFilterMode::Trilinear);

				m_textureHashMaps.addTexture(finalPath, std::move(texture));
				currentMaterial.m_ambientTexture = m_textureHashMaps.getTexture(finalPath);

				std::cout << "Loaded Ambient Texture: " << texturePath << std::endl;
			}
			else
			{
				currentMaterial.m_ambientTexture = m_textureHashMaps.getTexture(finalPath);
			}
		}

		if (str == "map_Kd")
		{
			std::string texturePath;
			ifs >> texturePath;

			const std::string path = R"(Meshes\sponza\)";
			const std::string finalPath = path + texturePath;

			if (m_textureHashMaps.getTexture(finalPath) == nullptr)
			{
				Texture texture(finalPath, TextureTarget::Texture2D, TextureWrapMode::Repeat, TextureFilterMode::Trilinear);

				m_textureHashMaps.addTexture(finalPath, std::move(texture));
				currentMaterial.m_diffuseTexture = m_textureHashMaps.getTexture(finalPath);

				std::cout << "Loaded Diffuse Texture: " << texturePath << std::endl;
			}
			else
			{
				currentMaterial.m_diffuseTexture = m_textureHashMaps.getTexture(finalPath);
			}
		}

		if (str == "map_Ks")
		{
			std::string texturePath;
			ifs >> texturePath;

			const std::string path = R"(Meshes\sponza\)";
			const std::string finalPath = path + texturePath;

			if (m_textureHashMaps.getTexture(finalPath) == nullptr)
			{
				Texture texture(finalPath, TextureTarget::Texture2D, TextureWrapMode::Repeat, TextureFilterMode::Trilinear);

				m_textureHashMaps.addTexture(finalPath, std::move(texture));
				currentMaterial.m_specularTexture = m_textureHashMaps.getTexture(finalPath);

				std::cout << "Loaded Specular Texture: " << texturePath << std::endl;
			}
			else
			{
				currentMaterial.m_specularTexture = m_textureHashMaps.getTexture(finalPath);
			}
		}

		if (str == "map_normal")
		{
			std::string texturePath;
			ifs >> texturePath;

			const std::string path = R"(Meshes\sponza\)";
			const std::string finalPath = path + texturePath;

			if (m_textureHashMaps.getTexture(finalPath) == nullptr)
			{
				Texture texture(finalPath, TextureTarget::Texture2D, TextureWrapMode::Repeat, TextureFilterMode::Trilinear);

				m_textureHashMaps.addTexture(finalPath, std::move(texture));
				currentMaterial.m_normalMapTexture = m_textureHashMaps.getTexture(finalPath);

				std::cout << "Loaded Normal Texture: " << texturePath << std::endl;
			}
			else
			{
				currentMaterial.m_normalMapTexture = m_textureHashMaps.getTexture(finalPath);
			}
		}

		if (str == "map_d")
		{
			std::string texturePath;
			ifs >> texturePath;

			const std::string path = R"(Meshes\sponza\)";
			const std::string finalPath = path + texturePath;

			if (m_textureHashMaps.getTexture(finalPath) == nullptr)
			{
				Texture texture(finalPath, TextureTarget::Texture2D, TextureWrapMode::Repeat, TextureFilterMode::Trilinear);

				m_textureHashMaps.addTexture(finalPath, std::move(texture));
				currentMaterial.m_maskTexture = m_textureHashMaps.getTexture(finalPath);

				std::cout << "Loaded Mask Texture: " << texturePath << std::endl;
			}
			else
			{
				currentMaterial.m_maskTexture = m_textureHashMaps.getTexture(finalPath);
			}
		}

		if (str == "Ns")
		{
			float shininess;
			ifs >> shininess;
			currentMaterial.m_shininess = shininess;
		}
	}

	// The last material will get canceled out due to ending the while loop
	if (currentMaterial.m_normalMapTexture == nullptr) { provideNormalMapTexture(currentMaterial); };
	if (currentMaterial.m_maskTexture == nullptr) { provideMaskTexture(currentMaterial); };

	m_Materials.insert({ materialName, currentMaterial }); 
}

const Material * MaterialReader::getMaterial(const std::string & materialName) const
{
	return &m_Materials.at(materialName);
}

void MaterialReader::provideNormalMapTexture(Material& material)
{
	const std::string path = R"(Meshes\sponza\textures\dummy_ddn.png)";
	material.m_normalMapTexture = m_textureHashMaps.getTexture(path);
}

void MaterialReader::provideMaskTexture(Material& material)
{
	const std::string path = R"(Meshes\sponza\textures\dummy_mask.png)";
	material.m_maskTexture = m_textureHashMaps.getTexture(path);
}


