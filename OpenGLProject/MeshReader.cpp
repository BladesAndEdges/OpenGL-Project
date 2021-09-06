#include "MeshReader.h"
#include <fstream>
#include <iostream>

MeshReader::MeshReader(const std::string & fileName, const OBJTYPE& objType)
{
	switch (objType)
	{
	case OBJTYPE::POSITIVE:
		parseFileWithPositiveIndices(fileName);
		break;
	case OBJTYPE::NEGATIVE:
		parseFileWithNegativeIndices(fileName);
		break;
	default:
		throw std::exception("Incorrect Object file type!");
	}
}

void MeshReader::parseFileWithPositiveIndices(const std::string & fileName)
{
	std::ifstream ifs(fileName);

	if (!ifs.is_open())
	{
		throw std::exception("Failed to open OBJ file for reading!");
	}

	std::string prefix;

	while (ifs >> prefix)
	{
		if (prefix == "v")
		{
			float x;
			float y;
			float z;

			ifs >> x >> y >> z;

			glm::vec3 position(x, y, z);
			m_vertexCoordinates.push_back(position);
		}

		if (prefix == "vt")
		{
			float u;
			float v;

			ifs >> u >> v;

			glm::vec2 textureCoordinate(u, v);
			m_textureCoordinates.push_back(textureCoordinate);
		}

		if (prefix == "vn")
		{
			float x;
			float y;
			float z;

			ifs >> x >> y >> z;

			glm::vec3 normal(x, y, z);
			m_normals.push_back(normal);
		}

		//if (prefix == "f")
		//{

		//}
	}
}

void MeshReader::parseFileWithNegativeIndices(const std::string & fileName)
{
}
