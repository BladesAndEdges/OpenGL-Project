#include "UniformBuffer.h"

// --------------------------------------------------------------------------------
void copyMat4ToFloatArray(const glm::mat4 & source, float destination[16])
{
	const float* matrixData = glm::value_ptr(source);

	for (int element = 0; element < 16; element++)
	{
		destination[element] = matrixData[element];
	}
}

// --------------------------------------------------------------------------------
void copyVec4ToFloatArray(const glm::vec4 & source, float destination[4])
{
	const float* vectorData = &source[0];

	for (int element = 0; element < 4; element++)
	{
		destination[element] = vectorData[element];
	}
}

// --------------------------------------------------------------------------------
void copyBoolIntoBuffer(const bool source, bool& destination)
{
	destination = source;
}

// --------------------------------------------------------------------------------
UniformBuffer::UniformBuffer() : normalMapToggle(1), ambientToggle(1), diffuseToggle(1), specularToggle(1),
lightSourceDirection{ 0.0f, 1001.0f, 0.0f, 0.0f }
{
}
