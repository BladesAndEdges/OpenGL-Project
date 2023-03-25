#version 450 core

layout(binding = 0) uniform sampler2DArray shadowMapTexture;

in vec3 v2f_textureCoordinate;
out vec3 o_fragmentColour;

void main()
{
	const float depthValue = texture(shadowMapTexture, v2f_textureCoordinate).r;	
	o_fragmentColour = vec3(depthValue, depthValue, depthValue);
}