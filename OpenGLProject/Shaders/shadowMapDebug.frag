#version 450

layout(binding = 0) uniform sampler2DArray shadowMapTexture;

in vec2 textureCoordinate;

out vec3 fragmentColour;

// Array with 4 layers, array 3 being used, array layer 0.0f being querried in shader produces *BLACK* texture; - Expected
// Array with 4 layers, array 3 being used, array layer 1.0f being querried in shader produces *BLACK* texture; - Expected
// Array with 4 layers, array 3 being used, array layer 2.0f being querried in shader produces *BLACK* texture; - Expected
// Array with 4 layers, array 3 being used, array layer 3.0f being querried in shader produces *SHADOW MAP* texture; - Expected

void main()
{
	const float depthValue = texture(shadowMapTexture, vec3(textureCoordinate, 3.0f)).r;
	
	fragmentColour = vec3(depthValue, depthValue, depthValue);
}