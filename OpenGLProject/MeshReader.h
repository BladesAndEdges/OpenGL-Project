#pragma once

#include <string>
#include <glm.hpp>
#include <vector>
#include "Face.h"

enum class OBJTYPE
{
	NEGATIVE,
	POSITIVE
};

class MeshReader
{

private:

	std::vector<glm::vec3> m_vertexPositions;
	std::vector<glm::vec2> m_textureCoordinates;
	std::vector<glm::vec3> m_normals;

	std::vector<Face> m_faces;

public:

	MeshReader(const std::string& fileName);

	void parseFile(const std::string& fileName);

	std::vector<Vertex> parseVertexData(const std::string& line);
	std::vector<Face> triangulateFaceVertices(const std::vector<Vertex>& vertices);

	std::vector<Face> getFaces() const;
	unsigned int getSizeOfFaceArray() const;

};

