#include "Texture.h"

#define TEXTURE_MAX_ANISOTROPY_EXT 0x84FE

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

	TextureFormat format = chooseTextureSizedFormat(numberOfChannels);

	GLenum glSizedFormat = translateFormatToOpenGLSizedFormat(format);
	GLenum glInternalFormat = translateFormatToOpenGLInternalFormat(format);

	if (textureData)
	{
		const uint32_t smallestSide = m_width > m_height ? m_height : m_width;

		const uint32_t mipCount = (uint32_t)ceilf(log2f((float)smallestSide));

		glTextureStorage2D(m_name, mipCount, glSizedFormat, m_width, m_height);
		glTextureSubImage2D(m_name, 0, 0, 0, m_width, m_height, glInternalFormat, GL_UNSIGNED_BYTE, textureData);
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
Texture::Texture(const std::string& label, uint32_t width, uint32_t height, TextureTarget target,
	TextureWrapMode wrapMode, TextureFilterMode filterMode, TextureFormat format)
{
	const GLenum glTarget = translateTargetToOpenGL(target);
	const GLenum glWrapMode = translateWrapModeToOpenGL(wrapMode);
	const GLenum glMinFilterMode = translateFilterModeToOpenGLMinFilter(filterMode);
	const GLenum glMagFilterMode = translateFilterModeToOpenGLMagFilter(filterMode);
	const GLenum glSizedFormat = translateFormatToOpenGLSizedFormat(format);

	glCreateTextures(glTarget, 1, &m_name);
	assert(m_name > 0);

	m_width = width;
	m_height = height;

	glTextureStorage2D(m_name, 1, glSizedFormat, m_width, m_height);

	glObjectLabel(GL_TEXTURE, m_name, -1, label.c_str());

	glTextureParameteri(m_name, GL_TEXTURE_WRAP_S, glWrapMode);
	glTextureParameteri(m_name, GL_TEXTURE_WRAP_T, glWrapMode);
	glTextureParameteri(m_name, GL_TEXTURE_MIN_FILTER, glMinFilterMode);
	glTextureParameteri(m_name, GL_TEXTURE_MAG_FILTER, glMagFilterMode);
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

// --------------------------------------------------------------------------------
Texture::Texture(Texture && other)
	: m_name(other.m_name),
	m_width(other.m_width),
	m_height(other.m_height)
{
	other.m_name = 0;
	other.m_width = 0;
	other.m_height = 0;
}

// --------------------------------------------------------------------------------
void Texture::useTexture(GLuint textureUnit) const
{
	glBindTextureUnit(textureUnit, m_name);
}

// --------------------------------------------------------------------------------
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
GLenum Texture::translateFormatToOpenGLSizedFormat(TextureFormat format) const
{
	switch (format)
	{
	case TextureFormat::R8:
		return GL_R8;

	case TextureFormat::RG8:
		return GL_RG8;

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
GLenum Texture::translateFormatToOpenGLInternalFormat(TextureFormat format) const
{
	switch (format)
	{
	case TextureFormat::R8:
		return GL_RED;

	case TextureFormat::RG8:
		return GL_RG;

	case TextureFormat::RGB8:
		return GL_RGB;

	case TextureFormat::RGBA8:
		return GL_RGBA;

	case TextureFormat::DEPTH32:
		return GL_DEPTH_COMPONENT;

	default:
		assert(false);
		return 0;
		break;
	}
}

// --------------------------------------------------------------------------------
TextureFormat Texture::chooseTextureSizedFormat(int numberOfChannels) const
{
	const TextureFormat colourFormats[4] =
	{
		TextureFormat::R8,
		TextureFormat::RG8,
		TextureFormat::RGB8,
		TextureFormat::RGBA8,
	};

	const uint32_t index = (uint32_t)(numberOfChannels - 1);

	return colourFormats[index];
}
