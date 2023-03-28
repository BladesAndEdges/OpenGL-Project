#version 450 core

out vec3 o_fragmentColour;

void main()
{
	o_fragmentColour = vec3(gl_FragCoord.x / 1000.0f, gl_FragCoord.y / 1000.0f, gl_FragCoord.z / 1000.0f);
}