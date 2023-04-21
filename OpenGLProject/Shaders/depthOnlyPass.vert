#version 450 core

#define MAXIMUM_CASCADES 4

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_TextureCoordinate;

layout(std140) uniform sceneMatrices
{
	vec4 worldCameraPosition; 
	vec4 lightSourceDirection; 
	mat4 model;
	mat4 viewProjection;

	mat4 worldToShadowMapMatrices[MAXIMUM_CASCADES];
	
	vec4 cascadeSplitsEndDistances;
	float offsetScale;
	float shadowDrawDistance;
	float shadowFadeStartDistance;
	
	bool normalMapToggle;
	bool padding;
	bool diffuseToggle;
	bool specularToggle;
	
	bool cascadeDrawDistanceToggle;
}ubo;

out vec2 out_textureCoordinate;

void main()
{
	gl_Position = ubo.viewProjection * (ubo.model * vec4(in_Position, 1.0f));
	
	out_textureCoordinate = in_TextureCoordinate;
}