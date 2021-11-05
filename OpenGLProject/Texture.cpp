#include "Texture.h"

#define TEXTURE_MAX_ANISOTROPY_EXT 0x84FE

/*GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, or GL_LINEAR_MIPMAP_LINEAR*/

GLenum Texture::translateTargetToOpenGL(const TextureTarget & target) const
{
	GLenum result = 0;

	switch (target)
	{
	case TextureTarget::Texture2D:
		result = GL_TEXTURE_2D;
	}

	return result;
}

Texture::Texture(const std::string & texturePath, const TextureTarget& textureTarget)
{
	const GLenum target = translateTargetToOpenGL(textureTarget);
	assert(target != 0);

	glCreateTextures(target, 1, &m_textureID);
	assert(m_textureID > 0);

	stbi_set_flip_vertically_on_load(true);

	GLint width;
	GLint height;
	GLint numberOfChannels;

	GLubyte* textureData = stbi_load(texturePath.c_str(), &width, &height, &numberOfChannels, 4); //Why does it only work when you specify as 4 channels?

	m_width = (uint32_t)width;
	m_height = (uint32_t)height;

	if (textureData)
	{
		glTextureStorage2D(m_textureID, 1, GL_RGBA8, m_width, m_height);
		glTextureSubImage2D(m_textureID, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
		stbi_image_free(textureData);
	}
	else
	{
		throw std::exception("Could not load texture image!");
	}

	glObjectLabel(GL_TEXTURE, m_textureID, -1, texturePath.c_str());

	glGenerateTextureMipmap(m_textureID);

	glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_textureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(m_textureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameterf(target, TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
}

Texture::Texture(const std::string & texturePath)
{
	static uint64_t textureDataUsage = 0;

	glGenTextures(1, &m_textureID);

	assert(m_textureID > 0);

	glBindTexture(GL_TEXTURE_2D, m_textureID);

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

void Texture::useTexture(GLuint textureUnit) const
{
	glBindTextureUnit(textureUnit, m_textureID);
}

GLuint Texture::getTextureID() const
{
	return m_textureID;
}


uint32_t Texture::getWidth() const
{
	return m_width;
}

uint32_t Texture::getHeight() const
{
	return m_height;
}

//Texture::~Texture()
//{
//	glDeleteTextures(1, &m_textureID);
//}
