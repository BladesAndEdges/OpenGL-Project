#include "Vertex.h"

// --------------------------------------------------------------------------------
Vertex::Vertex() : m_position{ 0.0f, 0.0f, 0.0f }
{
}

// --------------------------------------------------------------------------------
Vertex::Vertex(float x, float y, float z) : m_position{x, y, z}
{
}

// --------------------------------------------------------------------------------
bool operator==(const Vertex & lhs, const Vertex & rhs)
{
	bool positionEqual = false;
	bool texturesEqual = false;
	bool normalsEqual = false;

	for (unsigned int positionElement = 0; positionElement < 3; positionElement++)
	{
		// Position equality
		if (lhs.m_position[positionElement] == rhs.m_position[positionElement])
		{
			positionEqual = true;
		}
		else
		{
			positionEqual = false;
			break;
		}
	}

	// Texture Coordinate equality
	for (unsigned int textureElement = 0; textureElement < 2; textureElement++)
	{

		if (lhs.m_textureCoordinate[textureElement] == rhs.m_textureCoordinate[textureElement])
		{
			texturesEqual = true;
		}
		else
		{
			texturesEqual = false;
			break;
		}
	}

	// Normals equality
	for (unsigned int normalElement = 0; normalElement < 3; normalElement++)
	{
		// Position equality
		if (lhs.m_normal[normalElement] == rhs.m_normal[normalElement])
		{
			normalsEqual = true;
		}
		else
		{
			normalsEqual = false;
			break;
		}
	}

	return positionEqual && texturesEqual && normalsEqual;
}
