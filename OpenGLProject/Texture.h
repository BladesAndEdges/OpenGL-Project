#pragma once

#include "stb_image.h"
#include "glad.h"
#include <string>
#include <assert.h>

class Texture
{
	private:

		GLuint m_textureID;
		int width;
		int height;

	public:

		Texture();
		Texture(const std::string& texturePath);

		void useTexture() const;

		GLuint getTextureID() const;

		unsigned int getWidth();
		unsigned int getHeight();


		/*~Texture();*/
};

