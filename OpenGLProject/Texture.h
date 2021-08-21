#pragma once

#include "stb_image.h"
#include "glad.h"
#include <string>
#include <assert.h>



class Texture
{
	private:

		GLuint m_textureID;

	public:

		Texture(const std::string& texturePath);

		void useTexture() const;

		~Texture();
};

