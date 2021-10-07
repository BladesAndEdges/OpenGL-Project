#version 450

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_TextureCoordinate;
layout (location = 2) in vec3 in_Normal;


layout(std140) uniform sceneMatrices
{
	vec4 worldCameraPosition;
	mat4 model;
	mat4 viewProjection;
}ubo;

out vec3 out_worldSpaceNormal;
out vec3 out_worldSpaceFragment;
out vec2 out_textureCoordinate;

void main()
{
	gl_Position = ubo.viewProjection * (ubo.model * vec4(in_Position, 1.0f));
	
	out_worldSpaceNormal = vec3(ubo.model * vec4(in_Normal, 1.0f));
	
	out_worldSpaceFragment = vec3(ubo.model * vec4(in_Position, 1.0f));
	
	out_textureCoordinate = in_TextureCoordinate;
}