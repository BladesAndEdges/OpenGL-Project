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
