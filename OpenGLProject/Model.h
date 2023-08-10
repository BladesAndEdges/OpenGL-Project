#pragma once

#include <vector>
#include<glm.hpp> // Figure out how to forwardd declare vec3
#include "ModelParser.h"

struct Vertex;
struct Mesh;
class MaterialReader;

// --------------------------------------------------------------------------------
class Model
{
public:

	Model(const char* compiledModelFileName, const std::string& cacheSubFolder, const char* objSourceFile, const MaterialReader& materialReader);

	const std::vector<Mesh>& getMeshes() const;
	const std::vector<Vertex>& getIndexedVertexBuffer() const;
	const std::vector<unsigned int>& getIndexBuffer() const;
	glm::vec3 getSceneCenter() const;

	GLuint getVAO() const;

private:

	GLuint m_vao;

	std::vector<Mesh> m_meshes;
	std::vector<Vertex> m_indexedVertexBuffer;
	std::vector<unsigned int> m_indexBuffer;
	glm::vec3 m_sceneCenter;

	ModelParser m_modelParser;
};

