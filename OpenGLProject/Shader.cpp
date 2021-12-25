#include "Shader.h"

// --------------------------------------------------------------------------------
Shader::Shader(const char * const vertexShaderPath, const char * const fragmentShaderPath)
{
	std::string vertexShaderSource = readShaderSourceCode(vertexShaderPath);
	std::string fragmentShaderSource = readShaderSourceCode(fragmentShaderPath);

	GLuint vertexShader = compileShader(vertexShaderSource.c_str(), GL_VERTEX_SHADER);
	GLuint fragmentShader = compileShader(fragmentShaderSource.c_str(), GL_FRAGMENT_SHADER);

	linkShaders(vertexShader, fragmentShader);
}

// --------------------------------------------------------------------------------
std::string Shader::readShaderSourceCode(const char * const file) const
{
	std::string contents;

	std::ifstream ifs(file);
	bool isReady = ifs.is_open();
	assert(isReady);

	contents.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

	return contents;
}

// --------------------------------------------------------------------------------
GLuint Shader::compileShader(const char* const sourceCode, GLenum shaderType) const
{
	GLuint id;
	GLint success = GL_FALSE;
	char debugLog[512];

	id = glCreateShader(shaderType);
	glShaderSource(id, 1, &sourceCode, nullptr);
	glCompileShader(id);

	glGetShaderiv(id, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(id, 512, nullptr, debugLog);
		throw std::exception("Could not compile shader!");
	}

	return id;
}

// --------------------------------------------------------------------------------
void Shader::linkShaders(GLuint vertexShader, GLuint fragmentShader) 
{

	GLint success = GL_FALSE;
	char debugLog[512];

	m_programID = glCreateProgram();

	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);
	glLinkProgram(m_programID);

	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(m_programID, 512, nullptr, debugLog);
		throw std::exception("Could not link program!");
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

// --------------------------------------------------------------------------------
void Shader::useProgram() const
{
	glUseProgram(m_programID);
}

// --------------------------------------------------------------------------------
GLuint Shader::getProgramID() const
{
	return m_programID;
}
