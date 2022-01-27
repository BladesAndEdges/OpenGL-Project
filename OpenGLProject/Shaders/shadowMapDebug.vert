#version 450

const vec3 quadPositions[6]=vec3[6](
	vec3(-1.0f, 1.0f, 0.0f),
	vec3(-1.0f, 0.6f, 0.0f),
	vec3(-0.6f, 0.6f, 0.0f),
	vec3(-0.6f, 0.6f, 0.0f),
	vec3(-0.6f, 1.0f, 0.0f),
	vec3(-1.0f, 1.0f, 0.0f)
);

	const vec2 quadTextureCoordinates[6]=vec2[6](
	vec2(0.0f, 1.0f),
	vec2(0.0f, 0.0f),
	vec2(1.0f, 0.0f),
	vec2(1.0f, 0.0f),
	vec2(1.0f, 1.0f),
	vec2(0.0f, 1.0f)
);

out vec2 textureCoordinate;

void main()
{
	gl_Position = vec4(quadPositions[gl_VertexID], 1.0f);
	textureCoordinate = quadTextureCoordinates[gl_VertexID];
}