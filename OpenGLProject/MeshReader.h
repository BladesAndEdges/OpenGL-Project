#pragma once

#include <string>
#include <glm.hpp>
#include <vector>

#include "Mesh.h"
#include "Face.h"

#include "MaterialReader.h"
#include "mikktspace.h"

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

	SMikkTSpaceContext m_mikktSpaceContext;


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

// MikktSpace getters and setters
int getNumFaces(const SMikkTSpaceContext * pContext);
int getNumVerticesOfFace(const SMikkTSpaceContext*, const int);

// returns the position/normal/texcoord of the referenced face of vertex number iVert.
// iVert is in the range {0,1,2} for triangles and {0,1,2,3} for quads.
void getPosition(const SMikkTSpaceContext * pContext, float fvPosOut[], const int iFace, const int iVert);
void getNormal(const SMikkTSpaceContext * pContext, float fvNormOut[], const int iFace, const int iVert);
void getTexCoord(const SMikkTSpaceContext * pContext, float fvTexcOut[], const int iFace, const int iVert);

void setTSpaceBasic(const SMikkTSpaceContext * pContext, const float fvTangent[], const float fSign, const int iFace, const int iVert);
