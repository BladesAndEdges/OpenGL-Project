#include "Model.h"
#include "MaterialReader.h"
#include "ModelParser.h"
#include "ModelCacher.h"

#include <assert.h>

// --------------------------------------------------------------------------------
Model::Model(const char * compiledModelFileName, const std::string& cacheSubFolder, const char* objSourceFile, const MaterialReader& materialReader)
{
	const bool hasLoadedFromCache = ModelCacher::tryReadFromCache(compiledModelFileName, cacheSubFolder, materialReader, m_meshes, m_indexedVertexBuffer, m_indexBuffer);
	if (!hasLoadedFromCache)
	{
		const bool hasLoadedFromSource = m_modelParser.parseModelData(objSourceFile, materialReader, 
			m_meshes, m_indexedVertexBuffer, m_indexBuffer, m_sceneCenter);

		ModelCacher::writeToCache(compiledModelFileName,cacheSubFolder, m_meshes, m_indexedVertexBuffer, m_indexBuffer);
		if (!hasLoadedFromSource)
		{
			std::exception("Model data could not be loaded!");
		}
	}

	GLuint vbo;
	GLuint ibo;

	glCreateVertexArrays(1u, &m_vao);

	glCreateBuffers(1u, &vbo);
	glNamedBufferData(vbo, m_indexedVertexBuffer.size() * sizeof(Vertex), m_indexedVertexBuffer.data(), GL_STATIC_DRAW);

	glCreateBuffers(1u, &ibo);
	glNamedBufferData(ibo, m_indexBuffer.size() * sizeof(unsigned int), m_indexBuffer.data(), GL_STATIC_DRAW);

	glEnableVertexArrayAttrib(m_vao, 0u);
	glVertexArrayAttribBinding(m_vao, 0u, 0u);
	glVertexArrayAttribFormat(m_vao, 0u, 3u, GL_FLOAT, GL_FALSE, offsetof(Vertex, m_position));

	glEnableVertexArrayAttrib(m_vao, 1u);
	glVertexArrayAttribBinding(m_vao, 1u, 0u);
	glVertexArrayAttribFormat(m_vao, 1u, 2u, GL_FLOAT, GL_FALSE, offsetof(Vertex, m_textureCoordinate));

	glEnableVertexArrayAttrib(m_vao, 2u);
	glVertexArrayAttribBinding(m_vao, 2u, 0u);
	glVertexArrayAttribFormat(m_vao, 2u, 3u, GL_FLOAT, GL_FALSE, offsetof(Vertex, m_normal));

	glEnableVertexArrayAttrib(m_vao, 3u);
	glVertexArrayAttribBinding(m_vao, 3u, 0u);
	glVertexArrayAttribFormat(m_vao, 3u, 4u, GL_FLOAT, GL_FALSE, offsetof(Vertex, m_tangent));

	glVertexArrayVertexBuffer(m_vao, 0u, vbo, 0u, sizeof(Vertex));
	glVertexArrayElementBuffer(m_vao, ibo);
}

// --------------------------------------------------------------------------------
const std::vector<Mesh>& Model::getMeshes() const
{
	return m_meshes;
}

// --------------------------------------------------------------------------------
const std::vector<Vertex>& Model::getIndexedVertexBuffer() const
{
	return m_indexedVertexBuffer;
}

// --------------------------------------------------------------------------------
const std::vector<unsigned int>& Model::getIndexBuffer() const
{
	return m_indexBuffer;
}

// --------------------------------------------------------------------------------
glm::vec3 Model::getSceneCenter() const
{
	return m_sceneCenter;
}

// --------------------------------------------------------------------------------
GLuint Model::getVAO() const
{
	return m_vao;
}
