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
void updateUniformBuffer(UniformBuffer & ubo, const Camera & mainView, const Camera & shadowMapView, const glm::vec3& toLightDirectionWorldSpace, 
	float offScale, float shadowDrawDistance, float shadowFadeStart, bool nmToggle, bool ambToggle, bool diffToggle, bool specToggle, bool cascadeDrawDistanceToggle)
{
	const glm::vec4 worldSpacePosition = glm::vec4(mainView.getWorldPosition(), 1.0f);

	const glm::vec4 toLightDirectionWS = glm::vec4(toLightDirectionWorldSpace, 1.0f);

	const glm::mat4 model = glm::mat4(1.0f);
	const glm::mat4 view = mainView.createViewMatrix();
	const glm::mat4 projection = mainView.createProjectionMatrix();
	const glm::mat4 viewProjection = projection * view;

	const glm::mat4 shadowMapViewMatrix = shadowMapView.createViewMatrix();
	const glm::mat4 shadowMapProjectionMatrix = shadowMapView.createProjectionMatrix();
	const glm::mat4 shadowMapViewProjection = shadowMapProjectionMatrix * shadowMapViewMatrix;

	// Copy over values to the UBO
	copyVec4ToFloatArray(worldSpacePosition, ubo.worldSpaceCameraPosition);
	copyVec4ToFloatArray(toLightDirectionWS, ubo.lightSourceDirection);

	copyMat4ToFloatArray(model, ubo.model);
	copyMat4ToFloatArray(viewProjection, ubo.viewProjection);
	copyMat4ToFloatArray(shadowMapViewProjection, ubo.worldToShadowMap);

	ubo.offsetScale = offScale;
	ubo.shadowDrawDistance = shadowDrawDistance;
	ubo.shadowFadeStartDistance = shadowFadeStart * shadowDrawDistance;

	ubo.normalMapToggle = (nmToggle) ? 1 : 0;
	ubo.ambientToggle = (ambToggle) ? 1 : 0;
	ubo.diffuseToggle = (diffToggle) ? 1 : 0;
	ubo.specularToggle = (specToggle) ? 1 : 0;

	ubo.cascadeDrawDistanceOverlayToggle = (cascadeDrawDistanceToggle) ? 1 : 0;
}

// --------------------------------------------------------------------------------
void copyBoolIntoBuffer(const bool source, bool& destination)
{
	destination = source;
}

// --------------------------------------------------------------------------------
UniformBuffer::UniformBuffer() : lightSourceDirection{ 0.0f, 1001.0f, 0.0f, 0.0f }, normalMapToggle(1), ambientToggle(1), diffuseToggle(1), 
	specularToggle(1), cascadeDrawDistanceOverlayToggle(0)
{
}
