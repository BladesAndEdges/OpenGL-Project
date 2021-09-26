#include "MeshReader.h"
#include <fstream>
#include <iostream>
#include <sstream>


MeshReader::MeshReader(const std::string & objFileName, const std::string& materialFileName)
{
	m_materialReader.parseMaterialFile(materialFileName);
	parseMeshData(objFileName);
}

void MeshReader::parseMeshData(const std::string & fileName)
{
	std::ifstream ifs(fileName);

	if (!ifs.is_open())
	{
		throw std::exception("Could not read Mesh Data!");
	}

	std::string prefix;
	int count = 0;

	Mesh currentMesh;
	currentMesh.firstIndex = 0;
	currentMesh.vertexCount = 0;

	bool firstMesh = true;

	unsigned int materialsUsed = 0;

	while (ifs >> prefix)
	{
		if (prefix == "Object" || prefix == "object" || prefix == "o")
		{

			if (!firstMesh)
			{
				m_meshes.push_back(currentMesh);
			}

			currentMesh.firstIndex = m_faces.size() * 3;
			currentMesh.vertexCount = 0;

			firstMesh = false;
			materialsUsed = 0;
		}

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
			m_vertextextureCoordinates.push_back(textureCoordinate);
		}

		if (prefix == "vn")
		{
			float x;
			float y;
			float z;

			ifs >> x >> y >> z;

			glm::vec3 normal(x, y, z);
			m_vertexNormals.push_back(normal);
		}

		// This is done per line, so the total number of faces to compute the currentMesh needs to be re-iterated
		// I will need to consider doing a single Vertex array, which would make the levels of indirection here substantially easier
		if (prefix == "f")
		{
			std::string line;
			std::getline(ifs, line);

			const std::vector<Vertex> faceVertices = parseVertexData(line);
			const std::vector<Face> faces = triangulateFaceVertices(faceVertices);

			// Compute the vertex count for the mesh
			currentMesh.vertexCount += faces.size() * 3;

			m_faces.insert(std::end(m_faces), std::begin(faces), std::end(faces));
		}

		if (prefix == "usemtl")
		{
			std::string materialName;

			ifs >> materialName;

			if (materialsUsed == 0)
			{
				currentMesh.material = m_materialReader.getMaterial(materialName);
			}
			else
			{
				m_meshes.push_back(currentMesh);

				currentMesh.material = m_materialReader.getMaterial(materialName);
				currentMesh.firstIndex = m_faces.size() * 3;
				currentMesh.vertexCount = 0;
			}

			materialsUsed++;
		}
	}

	//If it's a single mesh within the Model, or for the final Mesh of a multi-mesh model
	m_meshes.push_back(currentMesh);
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

			std::string vertexTextureCoordinateIndex = "";
			if (firstBackSlashIndex != std::string::npos)
			{
				vertexTextureCoordinateIndex = vertexData.substr(firstBackSlashIndex + 1, secondBackSlashIndex - firstBackSlashIndex - 1);
			}

			std::string vertexNormalIndex = "";
			if (secondBackSlashIndex != std::string::npos)
			{
				vertexNormalIndex = vertexData.substr(secondBackSlashIndex + 1);
			}

			// Figure out what happens if a value is not supplied
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
			
				if (vertexPositionId >= 1)
				{
					vertex.m_position[0] = m_vertexPositions[vertexPositionId - 1].x;
					vertex.m_position[1] = m_vertexPositions[vertexPositionId - 1].y;
					vertex.m_position[2] = m_vertexPositions[vertexPositionId - 1].z;
				}
				else
				{
					unsigned int arraySize = m_vertexPositions.size();

					vertex.m_position[0] = m_vertexPositions[arraySize + vertexPositionId].x;
					vertex.m_position[1] = m_vertexPositions[arraySize + vertexPositionId].y;
					vertex.m_position[2] = m_vertexPositions[arraySize + vertexPositionId].z;
				}
			}

			//TO ADD: NEGATIVE INDICES
			if (vertexTextureCoordinateIndex != emptyString)
			{
				int vertexTextureCoordinateId = stoi(vertexTextureCoordinateIndex, nullptr);

				vertex.m_textureCoordinate[0] = m_vertextextureCoordinates[vertexTextureCoordinateId - 1].x;
				vertex.m_textureCoordinate[1] = m_vertextextureCoordinates[vertexTextureCoordinateId - 1].y;
			}

			if (vertexNormalIndex != emptyString)
			{
				int vertexNormalId = stoi(vertexNormalIndex, nullptr);

				vertex.m_normal[0] = m_vertexNormals[vertexNormalId - 1].x;
				vertex.m_normal[1] = m_vertexNormals[vertexNormalId - 1].y;
				vertex.m_normal[2] = m_vertexNormals[vertexNormalId - 1].z;
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
		Face face;

		face.A = vertices[0];
		face.B = vertices[triangle + 1];
		face.C = vertices[triangle + 2];

		faces.push_back(face);
	}

	return faces;
}

std::vector<Face> MeshReader::getFaces() const
{
	return m_faces;
}

std::vector<Mesh> MeshReader::getMeshes() const
{
	return m_meshes;
}

unsigned int MeshReader::getSizeOfFaceArray() const
{
	unsigned int size = 0;

	for (unsigned int face = 0; face < m_faces.size(); face++)
	{
		size += sizeof(Face);
	}

	return size;
}

const MaterialReader & MeshReader::getMaterialReader() const
{
	return m_materialReader;
}
