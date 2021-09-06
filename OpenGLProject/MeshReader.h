#pragma once

#include <string>
#include <glm.hpp>
#include <vector>

struct Vertex
{

};

enum class OBJTYPE
{
	NEGATIVE,
	POSITIVE
};

class MeshReader
{

private:

	std::vector<glm::vec3> m_vertexCoordinates;
	std::vector<glm::vec2> m_textureCoordinates;
	std::vector<glm::vec3> m_normals;

public:

	MeshReader(const std::string& fileName, const OBJTYPE& objType);

	void parseFileWithPositiveIndices(const std::string& fileName);
	void parseFileWithNegativeIndices(const std::string& fileName);

};

