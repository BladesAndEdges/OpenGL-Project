#include "Texture.h"

#define TEXTURE_MAX_ANISOTROPY_EXT 0x84FE

// --------------------------------------------------------------------------------
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

// --------------------------------------------------------------------------------
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

// --------------------------------------------------------------------------------
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

// --------------------------------------------------------------------------------
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

// --------------------------------------------------------------------------------
GLenum Texture::translateTextureFormatToOpenGLSizedFormat(TextureFormat format) const
{
	switch (format)
	{
	case TextureFormat::RGB8:
		return GL_RGB8;

	case TextureFormat::RGBA8:
		return GL_RGBA8;

	case TextureFormat::DEPTH32:
		return GL_DEPTH_COMPONENT32;

	default:
		assert(false);
		return 0;
	}
}

// --------------------------------------------------------------------------------
Texture::Texture(const std::string & source, TextureTarget target, TextureWrapMode wrapMode, 
											TextureFilterMode filterMode)
{
	const GLenum glTarget = translateTargetToOpenGL(target);
	const GLenum glWrapMode = translateWrapModeToOpenGL(wrapMode);
	const GLenum glMinFilterMode = translateFilterModeToOpenGLMinFilter(filterMode);
	const GLenum glMagFilterMode = translateFilterModeToOpenGLMagFilter(filterMode);

	glCreateTextures(glTarget, 1, &m_name);
	assert(m_name > 0);

	stbi_set_flip_vertically_on_load(true); 

	GLint width;
	GLint height;
	GLint numberOfChannels;

	GLubyte* textureData = stbi_load(source.c_str(), &width, &height, &numberOfChannels, 0);

	m_width = (uint32_t)width;
	m_height = (uint32_t)height;

	if (textureData)
	{
		switch (numberOfChannels)
		{
		case 1:
			glTextureStorage2D(m_name, 1, GL_R8, m_width, m_height);
			glTextureSubImage2D(m_name, 0, 0, 0, m_width, m_height, GL_RED, GL_UNSIGNED_BYTE, textureData);
			stbi_image_free(textureData);
			break;

		case 2:
			glTextureStorage2D(m_name, 1, GL_RG8, m_width, m_height);
			glTextureSubImage2D(m_name, 0, 0, 0, m_width, m_height, GL_RG, GL_UNSIGNED_BYTE, textureData);
			stbi_image_free(textureData);
			break;

		case 3:
			glTextureStorage2D(m_name, 1, GL_RGB8, m_width, m_height);
			glTextureSubImage2D(m_name, 0, 0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, textureData);
			stbi_image_free(textureData);
			break;

		case 4:
			glTextureStorage2D(m_name, 1, GL_RGBA8, m_width, m_height);
			glTextureSubImage2D(m_name, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
			stbi_image_free(textureData);
			break;

		default:
			throw std::exception("The number of channels within the texture does not match one of the supported values!");
			break;
		}
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

Texture::Texture(const std::string& label, TextureTarget target, TextureWrapMode wrapMode, TextureFilterMode filterMode)
{
	const GLenum glTarget = translateTargetToOpenGL(target);
	const GLenum glWrapMode = translateWrapModeToOpenGL(wrapMode);
	const GLenum glMinFilterMode = translateFilterModeToOpenGLMinFilter(filterMode);
	const GLenum glMagFilterMode = translateFilterModeToOpenGLMagFilter(filterMode);

	glCreateTextures(glTarget, 1, &m_name);
	assert(m_name > 0);

	stbi_set_flip_vertically_on_load(true);

	GLint width;
	GLint height;
	GLint numberOfChannels;

	GLubyte* textureData = stbi_load(source.c_str(), &width, &height, &numberOfChannels, 0);

	m_width = (uint32_t)width;
	m_height = (uint32_t)height;

	if (textureData)
	{
		switch (numberOfChannels)
		{
		case 1:
			glTextureStorage2D(m_name, 1, GL_R8, m_width, m_height);
			glTextureSubImage2D(m_name, 0, 0, 0, m_width, m_height, GL_RED, GL_UNSIGNED_BYTE, textureData);
			stbi_image_free(textureData);
			break;

		case 2:
			glTextureStorage2D(m_name, 1, GL_RG8, m_width, m_height);
			glTextureSubImage2D(m_name, 0, 0, 0, m_width, m_height, GL_RG, GL_UNSIGNED_BYTE, textureData);
			stbi_image_free(textureData);
			break;

		case 3:
			glTextureStorage2D(m_name, 1, GL_RGB8, m_width, m_height);
			glTextureSubImage2D(m_name, 0, 0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, textureData);
			stbi_image_free(textureData);
			break;

		case 4:
			glTextureStorage2D(m_name, 1, GL_RGBA8, m_width, m_height);
			glTextureSubImage2D(m_name, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
			stbi_image_free(textureData);
			break;

		default:
			throw std::exception("The number of channels within the texture does not match one of the supported values!");
			break;
		}
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

// --------------------------------------------------------------------------------
Texture::~Texture()
{
	if (m_name != 0)
	{
		glDeleteTextures(1, &m_name);
		m_name = 0;
	}
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
