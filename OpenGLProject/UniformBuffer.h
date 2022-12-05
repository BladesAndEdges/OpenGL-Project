#pragma once

#define MAXIMUM_NUM_OF_CASCADES 4

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
	float worldToShadowMap[MAXIMUM_NUM_OF_CASCADES * 16]; // Good idea for a define, perhaps

	float cascadeSplitsEndDistances[4];
	float offsetScale;
	float shadowDrawDistance;
	float shadowFadeStartDistance;

	// Debug bools
	uint32_t normalMapToggle;
	uint32_t ambientToggle;
	uint32_t diffuseToggle;
	uint32_t specularToggle;

	// Overlay bools
	uint32_t cascadeDrawDistanceOverlayToggle;
};

// --------------------------------------------------------------------------------
void copyMat4ToFloat16Array(const glm::mat4& source, float destination[16]);
void copyMat4ToFloat64ArrayByIndex(const glm::mat4& source, float destination[64], uint32_t matrixStartIndex);
void copyVec4ToFloatArray(const glm::vec4& source, float destination[4]);

void updateUniformBuffer(UniformBuffer& ubo, const Camera& mainView, const Camera& shadowMapView, const float cascadeSplitEndDistance, const uint32_t worldToShadowMapArrayId, const glm::vec3& toLightDirectionWorldSpace,
	float offScale, float shadowDrawDistance, float shadowFadingStart, bool nmToggle, bool ambToggle, bool diffToggle, bool specToggle, bool cascadeDrawDistanceToggle);
