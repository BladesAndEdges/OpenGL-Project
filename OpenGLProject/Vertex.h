#pragma once


struct Vertex
{
	float m_position[3];
	float m_textureCoordinate[2];
	float m_normal[3];

	Vertex();
	Vertex(float x, float y, float z);
};

