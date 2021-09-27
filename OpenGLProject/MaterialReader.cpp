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
			//Duplicating textures here
			std::string texturePath;
			ifs >> texturePath;

			const std::string path = R"(Meshes\sponza\)";
			const std::string finalPath = path + texturePath;

			Texture texture(finalPath); // Does the destructor destroy all textures?

			m_textureHashMaps.addAmbientTexture(texture.getTextureID(), texture);
			currentMaterial.m_ambientTexture = m_textureHashMaps.getAmbientTexture(texture.getTextureID());
		}
	}

	m_Materials.insert({ materialName, currentMaterial }); // The last material will get canceled out due to ending the while loop
}

const Material * MaterialReader::getMaterial(const std::string & materialName) const
{
	return &m_Materials.at(materialName);
}

//Ns 10.0000
//Ni 1.5000
//d 1.0000
//Tr 0.0000
//Tf 1.0000 1.0000 1.0000
//illum 2
//Ka 1 1 1
//Kd 1 1 1
//Ks 0.0000 0.0000 0.0000
//Ke 0.0000 0.0000 0.0000
//map_Ka textures\sponza_thorn_diff.png
//map_Kd textures\sponza_thorn_diff.png
//map_d textures\sponza_thorn_mask.png
//map_bump textures\sponza_thorn_bump.png


