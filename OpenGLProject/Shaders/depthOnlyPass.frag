#version 450 core

in vec2 out_textureCoordinate;

// This works without calling mesh.material->useTexture() since bindings seem to persist
layout(binding = 3) uniform sampler2D maskTextureSampler;

void main()
{
	if(texture(maskTextureSampler, out_textureCoordinate).r < 0.5f) discard;
} 