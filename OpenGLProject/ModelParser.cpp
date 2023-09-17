#include "ModelParser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <assert.h>
#include "MaterialReader.h"

#include "ProfileMarker.h"

// --------------------------------------------------------------------------------
ModelParser::ModelParser()
{
}

// --------------------------------------------------------------------------------
bool ModelParser::parseModelData(const char * objSourceFile, const MaterialReader& materialReader, std::vector<Mesh>& meshes, std::vector<Vertex>& indexedVertexBuffer, 
	std::vector<unsigned int>& indexBuffer, glm::vec3& sceneCenter, const bool flipTexCoordsAlongV, const float scaleFactor)
{
	assert(objSourceFile != nullptr);
	assert((meshes.size() == 0) && (indexedVertexBuffer.size() == 0) && (indexBuffer.size() == 0));

	createMeshAndFaceBuffers(objSourceFile, materialReader, meshes, flipTexCoordsAlongV, scaleFactor);

	computeTangentVectors();

	createIndexBufferAndIndexedVertexBuffer(indexedVertexBuffer, indexBuffer);

	assert((m_faces.size() * 3) == indexBuffer.size());

	sceneCenter = computeSceneCenter(indexedVertexBuffer);

	return (meshes.size() > 0) && (indexedVertexBuffer.size() > 0) && (indexBuffer.size() > 0);
}

// --------------------------------------------------------------------------------
void ModelParser::createMeshAndFaceBuffers(const std::string & fileName , const MaterialReader& materialReader, std::vector<Mesh>& meshes, 
	const bool flipTexCoorsAlongV, const float scaleFactor)
{
	std::ifstream ifs(fileName);

	if (!ifs.is_open())
	{
		throw std::exception("Could not read Mesh Data!");
	}

	std::string prefix;

	Mesh currentMesh;
	currentMesh.firstIndex = 0;
	currentMesh.indicesCount = 0;

	bool firstMesh = true;

	unsigned int materialsUsed = 0;

	while (ifs >> prefix)
	{
		if (prefix == "Object" || prefix == "object" || prefix == "o")
		{

			if (!firstMesh && (currentMesh.indicesCount > 0))
			{
				meshes.push_back(currentMesh);
			}

			currentMesh.firstIndex = (uint32_t)(m_faces.size() * 3);
			currentMesh.indicesCount = 0;

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

			// Flip coordinates along V, for DirectX system
			if (flipTexCoorsAlongV)
			{
				v = 1.0f - v;
			}

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

			const std::vector<Vertex> faceVertices = parseVertexData(line, scaleFactor);
			const std::vector<Face> faces = triangulateFaceVertices(faceVertices);

			// Compute the vertex count for the mesh
			currentMesh.indicesCount += (unsigned int)(faces.size() * 3);
			m_faces.insert(std::end(m_faces), std::begin(faces), std::end(faces));
		}

		if (prefix == "usemtl")
		{
			std::string materialName;

			ifs >> materialName;

			if (materialsUsed == 0)
			{
				currentMesh.material = materialReader.getMaterial(materialName);
				currentMesh.materialName = materialName;
			}
			else
			{
				if (currentMesh.indicesCount > 0)
				{
					meshes.push_back(currentMesh);
				}

				currentMesh.material = materialReader.getMaterial(materialName);
				currentMesh.materialName = materialName;
				currentMesh.firstIndex = (uint32_t)(m_faces.size() * 3);
				currentMesh.indicesCount = 0;
			}

			materialsUsed++;
		}
	}

	//If it's a single mesh within the Model, or for the final Mesh of a multi-mesh model
	if (currentMesh.indicesCount > 0)
	{
		meshes.push_back(currentMesh);
	}
}

// --------------------------------------------------------------------------------
std::vector<Vertex> ModelParser::parseVertexData(const std::string & line, const float scaleFactor)
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
					vertex.m_position[0] = m_vertexPositions[vertexPositionId - 1].x * scaleFactor; 
					vertex.m_position[1] = m_vertexPositions[vertexPositionId - 1].y * scaleFactor;
					vertex.m_position[2] = m_vertexPositions[vertexPositionId - 1].z * scaleFactor;
				}
				else
				{
					unsigned int arraySize = (unsigned int)m_vertexPositions.size();

					vertex.m_position[0] = m_vertexPositions[arraySize + vertexPositionId].x * scaleFactor;
					vertex.m_position[1] = m_vertexPositions[arraySize + vertexPositionId].y * scaleFactor;
					vertex.m_position[2] = m_vertexPositions[arraySize + vertexPositionId].z * scaleFactor;
				}

				// Code for centering the camera position
			}

			//TO ADD: NEGATIVE INDICES
			if (vertexTextureCoordinateIndex != emptyString)
			{
				int vertexTextureCoordinateId = stoi(vertexTextureCoordinateIndex, nullptr);

				if (vertexTextureCoordinateId >= 1)
				{
					vertex.m_textureCoordinate[0] = m_vertextextureCoordinates[vertexTextureCoordinateId - 1].x;
					vertex.m_textureCoordinate[1] = m_vertextextureCoordinates[vertexTextureCoordinateId - 1].y;
				}
				else
				{
					unsigned int arraySize = (unsigned int)m_vertextextureCoordinates.size();

					vertex.m_textureCoordinate[0] = m_vertextextureCoordinates[arraySize + vertexTextureCoordinateId].x;
					vertex.m_textureCoordinate[1] = m_vertextextureCoordinates[arraySize + vertexTextureCoordinateId].y;
				}
			}

			if (vertexNormalIndex != emptyString)
			{
				int vertexNormalId = stoi(vertexNormalIndex, nullptr);

				if (vertexNormalId >= 1)
				{
					vertex.m_normal[0] = m_vertexNormals[vertexNormalId - 1].x;
					vertex.m_normal[1] = m_vertexNormals[vertexNormalId - 1].y;
					vertex.m_normal[2] = m_vertexNormals[vertexNormalId - 1].z;
				}
				else
				{
					unsigned int arraySize = (unsigned int)m_vertexNormals.size();

					vertex.m_normal[0] = m_vertexNormals[arraySize + vertexNormalId].x;
					vertex.m_normal[1] = m_vertexNormals[arraySize + vertexNormalId].y;
					vertex.m_normal[2] = m_vertexNormals[arraySize + vertexNormalId].z;
				}
			}

			vertices.push_back(vertex);
		}
	}

	return vertices;
}

// --------------------------------------------------------------------------------
std::vector<Face> ModelParser::triangulateFaceVertices(const std::vector<Vertex>& vertices)
{
	const int vertexCount = (int)vertices.size();

	std::vector<Face> faces;

	for (int triangle = 0; triangle < vertexCount - 2; triangle++)
	{
		Face face;

		face.m_faceVertices[0] = vertices[0];
		face.m_faceVertices[1] = vertices[triangle + 1];
		face.m_faceVertices[2] = vertices[triangle + 2];

		faces.push_back(face);
	}

	return faces;
}

// --------------------------------------------------------------------------------
void ModelParser::createIndexBufferAndIndexedVertexBuffer(std::vector<Vertex>& indexedVertexBuffer, std::vector<unsigned int>& indexBuffer)
{
	std::unordered_map<Vertex, unsigned int, KeyHasher> indices;
	unsigned int index = 0;

	for (unsigned int face = 0; face < m_faces.size(); face++)
	{
		for (unsigned int vertex = 0; vertex < 3; vertex++)
		{
			Vertex key = m_faces[face].m_faceVertices[vertex];
			std::unordered_map<Vertex, unsigned int, KeyHasher>::iterator it = indices.find(key);

			if (it == indices.end())
			{
				indices.insert({ key, index });
				indexBuffer.push_back(index);
				indexedVertexBuffer.push_back(key);
				index++;
			}
			else
			{
				indexBuffer.push_back(it->second);
			}
		}
	}
}

// --------------------------------------------------------------------------------
glm::vec3 ModelParser::computeSceneCenter(std::vector<Vertex>& indexedVertexBuffer)
{
	float minX = indexedVertexBuffer[0].m_position[0];
	float maxX = minX;

	float minY = indexedVertexBuffer[0].m_position[1];
	float maxY = minY;

	float minZ = indexedVertexBuffer[0].m_position[2];
	float maxZ = minZ;

	// Start from 1 since the initial vertex is used as a starting value
	for (unsigned int vertex = 1; vertex < indexedVertexBuffer.size(); vertex++)
	{
		const float vertexX = indexedVertexBuffer[vertex].m_position[0];
		const float vertexY = indexedVertexBuffer[vertex].m_position[1];
		const float vertexZ = indexedVertexBuffer[vertex].m_position[2];

		if (vertexX < minX)
		{
			minX = vertexX;
		}
		
		if (vertexX > maxX)
		{
			maxX = vertexX;
		}

		if (vertexY < minY)
		{
			minY = vertexY;
		}

		if (vertexY > maxY)
		{
			maxY = vertexY;
		}

		if (vertexZ < minZ)
		{
			minZ = vertexZ;
		}

		if (vertexZ > maxZ)
		{
			maxZ = vertexZ;
		}
	}

 	const float halfwayX = (minX + maxX) / 2.0f;
	const float halfwayY = (minY + maxY) / 2.0f;
	const float halfWayZ = (minZ + maxZ) / 2.0f;

	return glm::vec3(halfwayX, halfwayY, halfWayZ);
}

// --------------------------------------------------------------------------------
void ModelParser::computeTangentVectors()
{
	SMikkTSpaceContext* mikkTSpaceContext = new SMikkTSpaceContext();
	SMikkTSpaceInterface* mikkTSpaceInterface = new SMikkTSpaceInterface();

	mikkTSpaceInterface->m_getPosition = &getPosition;
	mikkTSpaceInterface->m_getTexCoord = &getTexCoord;
	mikkTSpaceInterface->m_getNormal = &getNormal;
	mikkTSpaceInterface->m_getNumFaces = &getNumFaces;
	mikkTSpaceInterface->m_getNumVerticesOfFace = &getNumVerticesOfFace;

	mikkTSpaceInterface->m_setTSpaceBasic = &setTSpaceBasic;

	mikkTSpaceContext->m_pInterface = mikkTSpaceInterface;
	mikkTSpaceContext->m_pUserData = &m_faces;

	bool result = genTangSpaceDefault(mikkTSpaceContext);
	assert(result == true);

	const std::vector<Face>* fp = (std::vector<Face>*)mikkTSpaceContext->m_pUserData;
	m_faces = *fp;
}

// --------------------------------------------------------------------------------
int getNumFaces(const SMikkTSpaceContext * pContext)
{
	const std::vector<Face>* const fp = (const std::vector<Face>* const)pContext->m_pUserData;
	return (int)fp->size();
}

// --------------------------------------------------------------------------------
int getNumVerticesOfFace(const SMikkTSpaceContext*, const int)
{
	return 3;
}

// --------------------------------------------------------------------------------
void getPosition(const SMikkTSpaceContext * pContext, float fvPosOut[], const int iFace, const int iVert)
{
	const std::vector<Face>* const fp = (const std::vector<Face>* const)pContext->m_pUserData;
	
	fvPosOut[0] = (*fp)[iFace].m_faceVertices[iVert].m_position[0];
	fvPosOut[1] = (*fp)[iFace].m_faceVertices[iVert].m_position[1];
	fvPosOut[2] = (*fp)[iFace].m_faceVertices[iVert].m_position[2];
}

// --------------------------------------------------------------------------------
void getNormal(const SMikkTSpaceContext * pContext, float fvNormOut[], const int iFace, const int iVert)
{
	const std::vector<Face>* const fp = (const std::vector<Face>* const)pContext->m_pUserData;

	fvNormOut[0] = (*fp)[iFace].m_faceVertices[iVert].m_normal[0];
	fvNormOut[1] = (*fp)[iFace].m_faceVertices[iVert].m_normal[1];
	fvNormOut[2] = (*fp)[iFace].m_faceVertices[iVert].m_normal[2];
}

// --------------------------------------------------------------------------------
void getTexCoord(const SMikkTSpaceContext * pContext, float fvTexcOut[], const int iFace, const int iVert)
{
	const std::vector<Face>* const fp = (const std::vector<Face>* const)pContext->m_pUserData;

	fvTexcOut[0] = (*fp)[iFace].m_faceVertices[iVert].m_textureCoordinate[0];
	fvTexcOut[1] = (*fp)[iFace].m_faceVertices[iVert].m_textureCoordinate[1];
}

// --------------------------------------------------------------------------------
void setTSpaceBasic(const SMikkTSpaceContext * pContext, const float fvTangent[], const float fSign, const int iFace, const int iVert)
{
	std::vector<Face>* const fp = (std::vector<Face>* const)pContext->m_pUserData;

	(*fp)[iFace].m_faceVertices[iVert].m_tangent[0] = fvTangent[0];
	(*fp)[iFace].m_faceVertices[iVert].m_tangent[1] = fvTangent[1];
	(*fp)[iFace].m_faceVertices[iVert].m_tangent[2] = fvTangent[2];
	(*fp)[iFace].m_faceVertices[iVert].m_tangent[3] = fSign;
}
