#version 450

const vec3 quadPositions[24]=vec3[24](
	vec3(-1.0f, 1.0f, -1.0f), // Quad 0
	vec3(-1.0f, 0.6f, -1.0f),
	vec3(-0.6f, 0.6f, -1.0f),
	vec3(-0.6f, 0.6f, -1.0f),
	vec3(-0.6f, 1.0f, -1.0f),
	vec3(-1.0f, 1.0f, -1.0f),
	
	vec3(0.6f, 1.0f, -1.0f), // Quad 1
	vec3(0.6f, 0.6f, -1.0f),
	vec3(1.0f, 0.6f, -1.0f),
	vec3(1.0f, 0.6f, -1.0f),
	vec3(1.0f, 1.0f, -1.0f),
	vec3(0.6f, 1.0f, -1.0f),
	
	vec3(0.6f, -0.6f, -1.0f), // Quad 2
	vec3(0.6f, -1.0f, -1.0f),
	vec3(1.0f, -1.0f, -1.0f),
	vec3(1.0f, -1.0f, -1.0f),
	vec3(1.0f, -0.6f, -1.0f),
	vec3(0.6f, -0.6f, -1.0f),
	
	vec3(-1.0f, -0.6f, -1.0f), // Quad 3
	vec3(-1.0f, -1.0f, -1.0f),
	vec3(-0.6f, -1.0f, -1.0f),
	vec3(-0.6f, -1.0f, -1.0f),
	vec3(-0.6f, -0.6f, -1.0f), 
	vec3(-1.0f, -0.6f, -1.0f)
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

// Theory for why this currently never works is due to the function call
// The function call works like this: glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 4)
// The 6 represents the amount of vertices used
// The 4 represents the instanceCount
// My theory is that gl_VertexID more than likely never goes beyond 6
// Although now that I think of it, that might not be an issue??
void main()
{
	// 
	const uint vertexOffset = gl_VertexID % 6;
	const uint offsetInPositionArray = gl_InstanceID * 3 + vertexOffset;
	
	gl_Position = vec4(quadPositions[offsetInPositionArray], 1.0f);
	textureCoordinate = quadTextureCoordinates[vertexOffset];
}