#pragma once

#include <string>
//#include <glm.hpp>
#include <vector>

#include "ProfileMarker.h"
#include "Mesh.h"
#include "Face.h"
#include "mikktspace.h"

class MaterialReader;

// --------------------------------------------------------------------------------
class ModelParser
{
public:

	ModelParser();
	bool parseModelData(const char* objSourceFile, const MaterialReader& materialReader, std::vector<Mesh>& meshes, std::vector<Vertex>& indexedVertexBuffer, 
		std::vector<unsigned int>& indexBuffer, glm::vec3& sceneCenter, const bool flipTexCoordsAlongV, const float scaleFactor);

private:
	void createMeshAndFaceBuffers(const std::string & fileName, const MaterialReader& materialReader, std::vector<Mesh>& meshes, const bool flipTexCoordsAlongV, const float scaleFactor);

	void createIndexBufferAndIndexedVertexBuffer(std::vector<Vertex>& indexedVertexBuffer, std::vector<unsigned int>& indexBuffer);

	glm::vec3 computeSceneCenter(std::vector<Vertex>& indexedVertexBuffer);

	void computeTangentVectors();
	std::vector<Vertex> parseVertexData(const std::string& line, const float scaleFactor);
	std::vector<Face> triangulateFaceVertices(const std::vector<Vertex>& vertices);

	std::vector<glm::vec3> m_vertexPositions;
	std::vector<glm::vec2> m_vertextextureCoordinates;
	std::vector<glm::vec3> m_vertexNormals;
	std::vector<Face> m_faces;

	SMikkTSpaceContext m_mikktSpaceContext;
};

// --------------------------------------------------------------------------------
// MikktSpace getters and setters
int getNumFaces(const SMikkTSpaceContext * pContext);
int getNumVerticesOfFace(const SMikkTSpaceContext*, const int);

// returns the position/normal/texcoord of the referenced face of vertex number iVert.
// iVert is in the range {0,1,2} for triangles and {0,1,2,3} for quads.
void getPosition(const SMikkTSpaceContext * pContext, float fvPosOut[], const int iFace, const int iVert);
void getNormal(const SMikkTSpaceContext * pContext, float fvNormOut[], const int iFace, const int iVert);
void getTexCoord(const SMikkTSpaceContext * pContext, float fvTexcOut[], const int iFace, const int iVert);

void setTSpaceBasic(const SMikkTSpaceContext * pContext, const float fvTangent[], const float fSign, const int iFace, const int iVert);