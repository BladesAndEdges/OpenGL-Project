#version 450

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_TextureCoordinate;
layout (location = 2) in vec3 in_Normal;
layout (location = 3) in vec4 in_Tangent;


layout(std140) uniform sceneMatrices
{
	vec4 worldCameraPosition;
	vec4 lightSourceDirection; // Already normalized
	mat4 model;
	mat4 viewProjection;
	mat4 worldToShadowMap;
	
	float offsetScale;
	float boundingBoxDimensions;
	
	bool normalMapToggle;
	bool ambientToggle;
	bool diffuseToggle;
	bool specularToggle;
}ubo;


out vec3 out_worldSpaceNormal;
out vec3 out_worldSpaceFragment;
out vec2 out_textureCoordinate;
out vec4 out_Tangent;
out vec3 vn;

void main()
{
	gl_Position = ubo.viewProjection * (ubo.model * vec4(in_Position, 1.0f));
	
	out_worldSpaceNormal = vec3(ubo.model * vec4(in_Normal, 1.0f));
	
	out_worldSpaceFragment = vec3(ubo.model * vec4(in_Position, 1.0f));
	
	out_textureCoordinate = in_TextureCoordinate;
	
	out_Tangent = in_Tangent;
	
	vn = in_Normal;
}