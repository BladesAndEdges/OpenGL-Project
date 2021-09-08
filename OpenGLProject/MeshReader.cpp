#include "MeshReader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


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
			m_vertexPositions.push_back(position);
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

		if (prefix == "f")
		{
			std::string line;
			std::getline(ifs, line);

			const std::vector<Vertex> faceVertices = parseVertexData(line);
			const std::vector<Face> faces = triangulateFaceVertices(faceVertices);

			m_faces.insert(std::end(m_faces), std::begin(faces), std::end(faces));
		}
	}
}

void MeshReader::parseFileWithNegativeIndices(const std::string & fileName)
{
}

std::vector<Vertex> MeshReader::parseVertexData(const std::string & line)
{
	std::vector<Vertex> vertices;

	const char whitespace = ' ';

	std::istringstream lineStringStream(line);
	std::string vertexData;

	while (lineStringStream.good())
	{
		std::getline(lineStringStream, vertexData, whitespace);

		if (vertexData != "")
		{
			std::string backslash = "/";

			Vertex vertex;

			const size_t firstBackSlashIndex = vertexData.find(backslash);
			const size_t secondBackSlashIndex = vertexData.find(backslash, firstBackSlashIndex + 1);

			const std::string vertexPositionIndex = vertexData.substr(0, firstBackSlashIndex);
			const std::string vertexTextureCoordinateIndex = vertexData.substr(firstBackSlashIndex + 1, secondBackSlashIndex - firstBackSlashIndex - 1);
			const std::string vertexNormalIndex = vertexData.substr(secondBackSlashIndex + 1);

			glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec2 textureCoord = glm::vec2(0.0f, 0.0f);
			glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);

			const std::string emptyString = "";

			if (vertexPositionIndex != emptyString)
			{
				// You might be able to use the pointer to your advantage, do test it out
				// You can use this to track how far into the substring you are currently, and just simply 
				//continue extracting until this pointer is the end of the string

				int vertexPositionId = stoi(vertexPositionIndex, nullptr); 
			

				vertex.m_x = m_vertexPositions[vertexPositionId - 1].x;
				vertex.m_y = m_vertexPositions[vertexPositionId - 1].y;
				vertex.m_z = m_vertexPositions[vertexPositionId - 1].z;
			}

			if (vertexTextureCoordinateIndex != emptyString)
			{
				int vertexTextureCoordinateId = stoi(vertexTextureCoordinateIndex, nullptr);
			}

			if (vertexNormalIndex != emptyString)
			{
				int vertexNormalId = stoi(vertexNormalIndex, nullptr);
			}

			vertices.push_back(vertex);
		}
	}

	return vertices;
}

std::vector<Face> MeshReader::triangulateFaceVertices(const std::vector<Vertex>& vertices)
{
	const int vertexCount = vertices.size();

	std::vector<Face> faces;

	for (int triangle = 0; triangle < vertexCount - 2; triangle++)
	{
		Vertex pivot = vertices[0];
		Vertex firstNeighbour = vertices[triangle + 1];
		Vertex secondNeighbour = vertices[triangle + 2];

		Face face;

		face.vertices.push_back(pivot);
		face.vertices.push_back(firstNeighbour);
		face.vertices.push_back(secondNeighbour);

		faces.push_back(face);
	}

	return faces;
}