#version 450 core

in vec2 out_textureCoordinate;

layout(binding = 4) uniform sampler2D maskTextureSampler;

void main()
{
	if(texture(maskTextureSampler, out_textureCoordinate).r < 0.5f) discard;
} 