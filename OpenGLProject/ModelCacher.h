#pragma once

#include <vector>

struct Vertex;
struct Mesh;
class MaterialReader;

// --------------------------------------------------------------------------------
class ModelCacher
{
public:

	static bool tryReadFromCache(const char* fileName, const MaterialReader& materialReader, std::vector<Mesh>& meshes, std::vector<Vertex>& indexedVertexBuffer, 
		std::vector<unsigned int>& indexBuffer);

	static void writeToCache(const char* fileName, std::vector<Mesh>& meshes, std::vector<Vertex>& indexedVertexBuffer, 
		std::vector<unsigned int>& indexBuffer);
};

