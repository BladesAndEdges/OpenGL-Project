#version 450 core

in vec2 v2f_textureCoordinate;

layout(binding = 0) uniform sampler2D gBufferPositionSampler;

out vec3 o_fragmentColour;

void main()
{
	const vec2 c_screenCoordAsTexCoord = vec2(gl_FragCoord.x / 800, gl_FragCoord.y / 600);
	o_fragmentColour = texture(gBufferPositionSampler, c_screenCoordAsTexCoord).rgb;
}