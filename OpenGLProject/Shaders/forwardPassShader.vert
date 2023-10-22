#version 450 core

#define MAXIMUM_CASCADES 4

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_TextureCoordinate;
layout (location = 2) in vec3 in_Normal;
layout (location = 3) in vec4 in_Tangent;

layout(std140, binding = 6) uniform sceneMatrices
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

out vec3 v2f_worldSpaceNormal;
out vec3 v2f_worldSpaceFragment;
out vec2 v2f_textureCoordinate;
out vec4 v2f_Tangent;
out vec3 v2f_objectSpaceNormal;

void main()
{
	gl_Position = ubo.viewProjection * (ubo.model * vec4(in_Position, 1.0f));
	
	v2f_worldSpaceNormal = vec3(ubo.model * vec4(in_Normal, 1.0f));
	
	v2f_worldSpaceFragment = vec3(ubo.model * vec4(in_Position, 1.0f));
	
	v2f_textureCoordinate = in_TextureCoordinate;
	
	v2f_Tangent = in_Tangent;
	
	v2f_objectSpaceNormal = in_Normal;
}