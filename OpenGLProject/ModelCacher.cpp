#include "ModelCacher.h"
#include "ProfileMarker.h"
#include "Vertex.h"
#include "Mesh.h"
#include <assert.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <fstream>

#include "MaterialReader.h"

// --------------------------------------------------------------------------------
bool ModelCacher::tryReadFromCache(const char* fileName, const std::string& cacheSubFolder, const MaterialReader& materialReader, std::vector<Mesh>& meshes, std::vector<Vertex>& indexedVertexBuffer,
	std::vector<unsigned int>& indexBuffer, glm::vec3& sceneCenter)
{
	assert(fileName != nullptr);
	assert((meshes.size() == 0) && (indexedVertexBuffer.size() == 0) && (indexBuffer.size() == 0));

	const std::string cacheString = "Cache\\" + cacheSubFolder;
	const std::string finalPath = cacheString + std::string(fileName);
	assert(finalPath.size() != 0);
	
	std::ifstream inputStream;
	inputStream.open(finalPath, std::ios::binary);
	if (inputStream.is_open())
	{
		// Mesh Buffer
		uint32_t originalMeshBufferSize = 0u;
		inputStream.read((char*)&(originalMeshBufferSize), sizeof(uint32_t));
		meshes.resize(originalMeshBufferSize);
		for (Mesh& mesh : meshes)
		{
			inputStream.read((char*)&(mesh.firstIndex), sizeof(uint32_t));
			inputStream.read((char*)&(mesh.indicesCount), sizeof(uint32_t));

			uint32_t materialNameLength;
			inputStream.read((char*)&(materialNameLength), sizeof(uint32_t));

			std::vector<char> tempString(materialNameLength);
			inputStream.read(&tempString[0], materialNameLength);

			assert(tempString.size() == materialNameLength);

			mesh.materialName = std::string(tempString.begin(), tempString.end());
			mesh.material = materialReader.getMaterial(mesh.materialName);
		}
		
		// Vertex Buffer
		uint32_t originalIndexedVertexBufferSize = 0u;
		inputStream.read((char*)&(originalIndexedVertexBufferSize), sizeof(uint32_t));
		indexedVertexBuffer.resize(originalIndexedVertexBufferSize);
		inputStream.read((char*)(indexedVertexBuffer.data()), originalIndexedVertexBufferSize * sizeof(Vertex));
		
		// Index Buffer
		uint32_t originalIndexBufferSize = 0u;
		inputStream.read((char*)&(originalIndexBufferSize), sizeof(uint32_t));
		indexBuffer.resize(originalIndexBufferSize);
		inputStream.read((char*)(indexBuffer.data()), originalIndexBufferSize * sizeof(unsigned int));

		//Scene Center
		inputStream.read((char*)&(sceneCenter.x), sizeof(float));
		inputStream.read((char*)&(sceneCenter.y), sizeof(float));
		inputStream.read((char*)&(sceneCenter.z), sizeof(float));

		return true;
	}

	return false;
}

// --------------------------------------------------------------------------------
void ModelCacher::writeToCache(const char * fileName, const std::string& cacheSubFolder, std::vector<Mesh>& meshes, std::vector<Vertex>& indexedVertexBuffer, 
	std::vector<unsigned int>& indexBuffer, const glm::vec3& sceneCenter)
{
	assert(fileName != nullptr);
	assert((meshes.size() != 0) && (indexedVertexBuffer.size() != 0) && (indexBuffer.size() != 0));

	const std::string directoriesName = "Cache\\" + cacheSubFolder;
	const bool directoriesCreationSuccess = std::filesystem::create_directories(directoriesName);

	const std::string cacheString = "Cache\\" + cacheSubFolder;
	const std::string finalPath = cacheString + std::string(fileName);
	assert(finalPath.size() != 0);

	std::ofstream fw(finalPath, std::ios::binary);
	if (!fw)
	{
		std::cout << "Could create a file to cache the data!" << std::endl;
		assert(0);
	}

	// Meshes
	const uint32_t meshBufferSize = (uint32_t)meshes.size();
	fw.write((const char*)&(meshBufferSize), sizeof(uint32_t));
	for (const Mesh& mesh : meshes)
	{
		fw.write((const char*)&(mesh.firstIndex), sizeof(uint32_t));
		fw.write((const char*)&(mesh.indicesCount), sizeof(uint32_t));

		const uint32_t materialNameLength = (uint32_t)mesh.materialName.size();
		fw.write((const char*)&materialNameLength, sizeof(uint32_t));
		fw.write(mesh.materialName.data(), materialNameLength);
	}

	// Indexed Vertex Buffer
	const uint32_t indexedVertexBufferSize = (uint32_t)indexedVertexBuffer.size();
	fw.write((const char*)&(indexedVertexBufferSize), sizeof(uint32_t));
	fw.write((const char*)(indexedVertexBuffer.data()), indexedVertexBufferSize * sizeof(Vertex));

	// Index buffer
	const uint32_t indexBufferSize = (uint32_t)indexBuffer.size();
	fw.write((const char*)&(indexBufferSize), sizeof(uint32_t));
	fw.write((const char*)(indexBuffer.data()), indexBufferSize * sizeof(unsigned int)); // Change index bufferto use uint32_t

	// Scene center
	fw.write((const char*)&(sceneCenter.x), sizeof(float));
	fw.write((const char*)&(sceneCenter.y), sizeof(float));
	fw.write((const char*)&(sceneCenter.z), sizeof(float));

	fw.close();
}
