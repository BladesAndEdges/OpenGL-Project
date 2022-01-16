#version 450

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_TextureCoordinate;

layout(std140) uniform sceneMatrices
{
	vec4 worldCameraPosition;
	vec4 lightSourceDirection; // Already normalized
	mat4 model;
	mat4 viewProjection;
	mat4 worldToShadowMap;
	
	// Rule 1: Both the size and alignment are the size of the type in basic machine units.
	
	uint shadowMapTexelCount;
	
	bool pcfToggle;
	bool normalMapToggle;
	bool ambientToggle;
	bool diffuseToggle;
	bool specularToggle;
}ubo;

out vec2 out_textureCoordinate;

void main()
{
	gl_Position = ubo.viewProjection * (ubo.model * vec4(in_Position, 1.0f));
	
	out_textureCoordinate = in_TextureCoordinate;
}