#pragma once

#include <string>
#include <iostream>
#include <iomanip>
#include <functional>
#include <string>
#include <unordered_set>

struct Vertex
{
	float m_position[3];
	float m_textureCoordinate[2];
	float m_normal[3];

	float m_tangent[4];

	Vertex();
	Vertex(float x, float y, float z);
};

bool operator==(const Vertex& lhs, const Vertex& rhs);

// class for hash function 
struct KeyHasher
{
	std::size_t operator()(const Vertex& v) const
	{
		using std::size_t;
		using std::hash;
		using std::string;

		return ((hash<float>()(v.m_position[0])
			^ (hash<float>()(v.m_textureCoordinate[1]) << 1)) >> 1)
			^ (hash<float>()(v.m_normal[2]) << 1);
	}
};