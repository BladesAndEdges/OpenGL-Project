#include "Model.h"
#include "MaterialReader.h"
#include "ModelParser.h"
#include "ModelCacher.h"

#include <assert.h>

// --------------------------------------------------------------------------------
Model::Model(const char * compiledModelFileName, const char* objSourceFile, const MaterialReader& materialReader)
{
	const bool hasLoadedFromCache = ModelCacher::tryReadFromCache(compiledModelFileName, materialReader, m_meshes, m_indexedVertexBuffer, m_indexBuffer);
	if (!hasLoadedFromCache)
	{
		const bool hasLoadedFromSource = m_modelParser.parseModelData(objSourceFile, materialReader, 
			m_meshes, m_indexedVertexBuffer, m_indexBuffer, m_sceneCenter);

		ModelCacher::writeToCache(compiledModelFileName, m_meshes, m_indexedVertexBuffer, m_indexBuffer);
		if (!hasLoadedFromSource)
		{
			std::exception("Model data could not be loaded!");
		}
	}

	// Vertex Buffers and VAO
	unsigned int modelVBO;

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &modelVBO);

	glBindVertexArray(m_vao);

	// Index Buffer
	unsigned int modelIBO;
	glGenBuffers(1, &modelIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(unsigned int), m_indexBuffer.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
	glBufferData(GL_ARRAY_BUFFER, m_indexedVertexBuffer.size() * sizeof(Vertex), m_indexedVertexBuffer.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_textureCoordinate));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_tangent));
	glEnableVertexAttribArray(3);
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
