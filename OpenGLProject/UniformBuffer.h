#pragma once

#pragma warning(push)
#pragma warning(disable:4201)   
#include<glm.hpp>
#include<gtc/type_ptr.hpp>
#pragma warning(pop)

#include <cstring>



struct UniformBuffer
{
	float worldSpaceCameraPosition[4];
	float model[16];
	float viewProjection[16];
};

void copyMat4ToFloatArray(const glm::mat4& source, float destination[16]);
void copyVec4ToFloatArray(const glm::vec4& source, float destination[4]);

