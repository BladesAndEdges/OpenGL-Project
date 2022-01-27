#version 450

uniform sampler2D shadowMapTexture;

in vec2 textureCoordinate;

out vec3 fragmentColour;

void main()
{
	const float depthValue = texture(shadowMapTexture, textureCoordinate).r;
	fragmentColour = vec3(depthValue, depthValue, depthValue);
}