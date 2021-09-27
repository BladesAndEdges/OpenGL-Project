#include "MaterialReader.h"

#include <iostream>
#include <fstream>


MaterialReader::MaterialReader()
{
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
				m_Materials.insert({ materialName, currentMaterial });
			}

			// Reset pointers??
			currentMaterial.m_ambientTexture = nullptr;
			currentMaterial.m_diffuseTexture = nullptr;
			currentMaterial.m_specularTexture = nullptr;

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
				Texture texture(finalPath); // Does the destructor destroy all textures?

				m_textureHashMaps.addTexture(finalPath, texture);
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
				Texture texture(finalPath); // Does the destructor destroy all textures?

				m_textureHashMaps.addTexture(finalPath, texture);
				currentMaterial.m_diffuseTexture = m_textureHashMaps.getTexture(finalPath);

				std::cout << "Loaded Ambient Texture: " << texturePath << std::endl;
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
				Texture texture(finalPath); // Does the destructor destroy all textures?

				m_textureHashMaps.addTexture(finalPath, texture);
				currentMaterial.m_specularTexture = m_textureHashMaps.getTexture(finalPath);

				std::cout << "Loaded Ambient Texture: " << texturePath << std::endl;
			}
			else
			{
				currentMaterial.m_specularTexture = m_textureHashMaps.getTexture(finalPath);
			}
		}
	}

	m_Materials.insert({ materialName, currentMaterial }); // The last material will get canceled out due to ending the while loop
}

const Material * MaterialReader::getMaterial(const std::string & materialName) const
{
	return &m_Materials.at(materialName);
}


