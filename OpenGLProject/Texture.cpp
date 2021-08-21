#include "Texture.h"
#include <iostream>

Texture::Texture(const std::string & texturePath)
{
	glGenTextures(1, &m_textureID);

	glBindTexture(GL_TEXTURE_2D, m_textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int numberOfChannels;
	int width;
	int height;

	//Do I need to flip the coordinates using stbi_image_flip?
	stbi_set_flip_vertically_on_load(true);


	unsigned char* textureData = stbi_load(texturePath.c_str(), &width, &height, &numberOfChannels, 4); //Why does it only work when you specify as 4 channels?

	if (textureData) 
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE , textureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << stbi_failure_reason() << std::endl;
		throw std::exception("Could not load texture image!");
	}

	stbi_image_free(textureData);
}

void Texture::useTexture() const
{
	glBindTexture(GL_TEXTURE_2D, m_textureID);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_textureID);
}
