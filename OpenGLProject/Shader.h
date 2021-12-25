#pragma once

#include<string.h>
#include <glad.h>
#include <string>
#include <fstream>
#include <assert.h>
#include <exception>

// --------------------------------------------------------------------------------
class Shader
{

public:

	Shader(const char* const vertexShaderPath, const char* const fragmentShaderPath);

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	std::string readShaderSourceCode(const char* const file) const;

	GLuint compileShader(const char* const sourceCode, GLenum shaderType) const;
	void linkShaders(GLuint vertexShader, GLuint fragmentShader);
	void useProgram() const;

	GLuint getProgramID() const;

private:

	GLuint m_programID;

};

