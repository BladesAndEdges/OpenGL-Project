#pragma once

#pragma warning(push)
#pragma warning(disable:4201)   
#include<glm.hpp>
#include<gtc/type_ptr.hpp>
#pragma warning(pop)

#include <cstring>
#include "Camera.h"

// --------------------------------------------------------------------------------
struct UniformBuffer
{
	UniformBuffer();

	float worldSpaceCameraPosition[4];
	float lightSourceDirection[4];
	float model[16]; 
	float viewProjection[16];

	// 144 bytes
	uint32_t normalMapToggle;
	uint32_t ambientToggle;
	uint32_t diffuseToggle;
	uint32_t specularToggle;
};

// --------------------------------------------------------------------------------
void copyMat4ToFloatArray(const glm::mat4& source, float destination[16]);
void copyVec4ToFloatArray(const glm::vec4& source, float destination[4]);
void copyBoolIntoBuffer(const bool source, bool& destination);

