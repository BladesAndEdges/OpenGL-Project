#pragma once

#include "stb_image.h"
#include "glad.h"

#include <cstdint>
#include <string>
#include <assert.h>


enum class TextureTarget
{
	Texture2D
};

//-------------------------------------------------------------------------------------------
class Texture
{
	private:

		GLuint m_textureID;
		uint32_t m_width;
		uint32_t m_height;

		GLenum translateTargetToOpenGL(const TextureTarget& target) const;

	public:

		Texture(const std::string& textureSource);
		Texture(const std::string& textureSource, const TextureTarget& textureTarget);

		void useTexture(GLuint textureUnit) const;

		GLuint getTextureID() const;
		uint32_t getWidth() const;
		uint32_t getHeight() const;

};

