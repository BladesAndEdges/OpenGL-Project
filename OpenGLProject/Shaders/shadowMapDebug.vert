#version 450 core

const float c_quadSize = 0.2f;
const float c_quadSpacing = c_quadSize + 0.02f;

const vec3 c_quadPositions[6]=vec3[6](
	vec3(-1.0f, 1.0f, -1.0f), // Quad 0
	vec3(-1.0f, 1.0f - c_quadSize, -1.0f),
	vec3(-1.0f + c_quadSize, 1.0f - c_quadSize, -1.0f),
	vec3(-1.0f + c_quadSize, 1.0f - c_quadSize, -1.0f),
	vec3(-1.0f + c_quadSize, 1.0f, -1.0f),
	vec3(-1.0f, 1.0f, -1.0f)
);

const vec2 c_quadTextureCoordinates[6]=vec2[6](
	vec2(0.0f, 1.0f),
	vec2(0.0f, 0.0f),
	vec2(1.0f, 0.0f),
	vec2(1.0f, 0.0f),
	vec2(1.0f, 1.0f),
	vec2(0.0f, 1.0f)
);

out vec3 v2f_textureCoordinate;

void main()
{
	gl_Position = vec4(c_quadPositions[gl_VertexID], 1.0f);
	gl_Position.y -= gl_InstanceID * c_quadSpacing;
	v2f_textureCoordinate = vec3(c_quadTextureCoordinates[gl_VertexID], float(gl_InstanceID));	
}