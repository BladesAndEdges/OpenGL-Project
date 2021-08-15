#include "UniformBuffer.h"

void copyMat4ToFloatArray(const glm::mat4 & source, float destination[16])
{
	const float* matrixData = glm::value_ptr(source);

	for (int element = 0; element < 16; element++)
	{
		destination[element] = matrixData[element];
	}
}
