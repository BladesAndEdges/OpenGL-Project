#pragma once

#include <vector>
#include <glm.hpp>

struct Vertex;
struct Mesh;
class MaterialReader;

// --------------------------------------------------------------------------------
class ModelCacher
{
public:

	static bool tryReadFromCache(const char* fileName, const std::string& cacheSubFolder, const MaterialReader& materialReader, std::vector<Mesh>& meshes, std::vector<Vertex>& indexedVertexBuffer,
		std::vector<unsigned int>& indexBuffer, glm::vec3& sceneCenter);

	static void writeToCache(const char* fileName, const std::string& cacheSubFolder, std::vector<Mesh>& meshes, std::vector<Vertex>& indexedVertexBuffer, 
		std::vector<unsigned int>& indexBuffer, const glm::vec3& sceneCenter);
};

