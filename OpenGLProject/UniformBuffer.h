#pragma once

#include <glm.hpp>
#include <cstring>
#include <gtc/type_ptr.hpp>


struct UniformBuffer
{
	float worldSpaceCameraPosition[4];
	float model[16];
	float viewProjection[16];
};

void copyMat4ToFloatArray(const glm::mat4& source, float destination[16]);
void copyVec4ToFloatArray(const glm::vec4& source, float destination[4]);

