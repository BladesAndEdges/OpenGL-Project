#version 450

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_TextureCoordinate;

layout(std140) uniform sceneMatrices
{
	vec4 worldCameraPosition; // Change this to a vec3
	vec4 lightSourceDirection; // Already normalized
	mat4 model;
	mat4 viewProjection;
	mat4 worldToShadowMap;
	
	vec4 cascadeSplitsStartDistances;
	float offsetScale;
	float shadowDrawDistance;
	float shadowFadeStartDistance;
	
	bool normalMapToggle;
	bool ambientToggle;
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