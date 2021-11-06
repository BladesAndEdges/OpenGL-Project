#include "Texture.h"

#define TEXTURE_MAX_ANISOTROPY_EXT 0x84FE

GLenum Texture::translateTargetToOpenGL(TextureTarget target) const
{
	switch (target)
	{
	case TextureTarget::Texture2D:
		return GL_TEXTURE_2D;
	default:
		assert(false);
		return 0;
	}
}

GLenum Texture::translateWrapModeToOpenGL(TextureWrapMode wrapMode) const
{
	switch (wrapMode)
	{
	case TextureWrapMode::Repeat:
		return GL_REPEAT;
	case TextureWrapMode::ClampEdge:
		return GL_CLAMP_TO_EDGE;
	default:
		assert(false);
		return 0;
	}
}

GLenum Texture::translateFilterModeToOpenGLMinFilter(TextureFilterMode filterMode) const
{
	switch (filterMode)
	{
	case TextureFilterMode::Point:
		return GL_NEAREST_MIPMAP_NEAREST;
	case TextureFilterMode::Bilinear:
		return GL_LINEAR_MIPMAP_NEAREST;
	case TextureFilterMode::Trilinear:
		return GL_LINEAR_MIPMAP_LINEAR;
	default:
		assert(false);
		return 0;
	}
}

GLenum Texture::translateFilterModeToOpenGLMagFilter(TextureFilterMode filterMode) const
{
	switch (filterMode)
	{
	case TextureFilterMode::Point:
		return GL_NEAREST;
	case TextureFilterMode::Bilinear:
	case TextureFilterMode::Trilinear:
		return GL_LINEAR;
	default:
		assert(false);
		return 0;
	}
}

Texture::Texture(const std::string & source, TextureTarget target, TextureWrapMode wrapMode, 
											TextureFilterMode filterMode)
{
	const GLenum glTarget = translateTargetToOpenGL(target);
	const GLenum glWrapMode = translateWrapModeToOpenGL(wrapMode);
	const GLenum glMinFilterMode = translateFilterModeToOpenGLMinFilter(filterMode);
	const GLenum glMagFilterMode = translateFilterModeToOpenGLMagFilter(filterMode);

	glGetError();
	glCreateTextures(glTarget, 1, &m_name);
	assert(m_name > 0);

	stbi_set_flip_vertically_on_load(true); // ?

	GLint width;
	GLint height;
	GLint numberOfChannels;

	GLubyte* textureData = stbi_load(source.c_str(), &width, &height, &numberOfChannels, 0);

	m_width = (uint32_t)width;
	m_height = (uint32_t)height;

	if (textureData)
	{
		glTextureStorage2D(m_name, 1, GL_RGBA8, m_width, m_height);
		glTextureSubImage2D(m_name, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
		stbi_image_free(textureData);
	}
	else
	{
		throw std::exception("Could not load texture image!");
	}

	glObjectLabel(GL_TEXTURE, m_name, -1, source.c_str());

	glGenerateTextureMipmap(m_name);

	glTextureParameteri(m_name, GL_TEXTURE_WRAP_S, glWrapMode);
	glTextureParameteri(m_name, GL_TEXTURE_WRAP_T, glWrapMode);
	glTextureParameteri(m_name, GL_TEXTURE_MIN_FILTER, glMinFilterMode);
	glTextureParameteri(m_name, GL_TEXTURE_MAG_FILTER, glMagFilterMode);
	glTextureParameterf(m_name, TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
}

Texture::~Texture()
{
	if (m_name != 0)
	{
		glDeleteTextures(1, &m_name);
		m_name = 0;
	}
}

Texture::Texture(const std::string & texturePath)
{
	static uint64_t textureDataUsage = 0;

	glGenTextures(1, &m_name);

	assert(m_name > 0);

	glBindTexture(GL_TEXTURE_2D, m_name);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);

	int numberOfChannels;

	//Do I need to flip the coordinates using stbi_image_flip?
	stbi_set_flip_vertically_on_load(true);

	int width = (int)m_width;
	int height = (int)m_height;
	unsigned char* textureData = stbi_load(texturePath.c_str(), &width, &height, &numberOfChannels, 4); //Why does it only work when you specify as 4 channels?

	if (textureData) 
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE , textureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		throw std::exception("Could not load texture image!");
	}

	stbi_image_free(textureData);

	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(Texture && other) 
	: m_name(other.m_name),
	m_width(other.m_width),
	m_height(other.m_height)
{
	other.m_name = 0;
	other.m_width = 0;
	other.m_height = 0;
}

void Texture::useTexture(GLuint textureUnit) const
{
	glBindTextureUnit(textureUnit, m_name);
}

GLuint Texture::getName() const
{
	return m_name;
}

uint32_t Texture::getWidth() const
{
	return m_width;
}

uint32_t Texture::getHeight() const
{
	return m_height;
}
