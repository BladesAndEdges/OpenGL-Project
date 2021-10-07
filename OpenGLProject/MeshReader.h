#pragma once

#include <string>
#include <glm.hpp>
#include <vector>

#include "Mesh.h"
#include "Face.h"

#include "MaterialReader.h"

class MeshReader
{

private:

	std::vector<glm::vec3> m_vertexPositions;
	std::vector<glm::vec2> m_vertextextureCoordinates;
	std::vector<glm::vec3> m_vertexNormals;

	std::vector<Mesh> m_meshes;
	std::vector<Face> m_faces; // Essentially the vertex buffer of duplicates

	std::unordered_map<Vertex, unsigned int, KeyHasher> m_indices;
	std::vector<unsigned int> m_indexBuffer;
	std::vector<Vertex> m_indexedVertexBuffer;



	//// Unordered map of materials
	MaterialReader m_materialReader;

public:

	MeshReader(const std::string& objFileName , const std::string& materialFileName);
	
	void parseMeshData(const std::string& fileName);

	std::vector<Vertex> parseVertexData(const std::string& line);
	std::vector<Face> triangulateFaceVertices(const std::vector<Vertex>& vertices);

	const std::vector<Mesh>& getMeshes() const;
	const std::vector<unsigned int>& getIndexBuffer() const;
	const std::vector<Vertex>& getIndexedVertexBuffer() const;

	void createIndexBuffer();

	const MaterialReader& getMaterialReader() const;
};

