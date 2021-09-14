#pragma once

#include <string>
#include <glm.hpp>
#include <vector>

#include "Mesh.h"
#include "Face.h"

class MeshReader
{

private:

	std::vector<glm::vec3> m_vertexPositions;
	std::vector<glm::vec2> m_vertextextureCoordinates;
	std::vector<glm::vec3> m_vertexNormals;

	std::vector<Mesh> m_meshes;
	std::vector<Face> m_faces;

public:

	MeshReader(const std::string& fileName);

	void parseFile(const std::string& fileName);
	void parseMeshData(const std::string& fileName);
	void resetAttributeArrays();

	std::vector<Vertex> parseVertexData(const std::string& line);
	std::vector<Face> triangulateFaceVertices(const std::vector<Vertex>& vertices);

	std::vector<Face> getFaces() const;
	std::vector<Mesh> getMeshes() const;

	unsigned int getSizeOfMeshArray() const;
	unsigned int getSizeOfFaceArray() const; // In bytes
};

