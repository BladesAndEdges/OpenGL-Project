#pragma once

#include "stb_image.h"
#include "glad.h"

#include <cstdint>
#include <string>
#include <assert.h>

// ------------------------------------------------------------------------------------------
enum class TextureTarget
{
	Texture2D
};

// ------------------------------------------------------------------------------------------
enum class TextureWrapMode
{
	Repeat, 
	ClampEdge
};

// ------------------------------------------------------------------------------------------
// Inspired by Unity's version on doing this
enum class TextureFilterMode
{
	Point, 
	Bilinear,
	Trilinear
};

// ------------------------------------------------------------------------------------------
class Texture
{
	private:

		GLuint m_name;
		uint32_t m_width;
		uint32_t m_height;

		// Convert details of the texture to a form OpenGL understands
		GLenum translateTargetToOpenGL(TextureTarget target) const;
		GLenum translateWrapModeToOpenGL(TextureWrapMode wrapMode) const;
		GLenum translateFilterModeToOpenGLMinFilter(TextureFilterMode filterMode) const;
		GLenum translateFilterModeToOpenGLMagFilter(TextureFilterMode filterMode) const;

	public:

		Texture(const std::string& source);
		Texture(const std::string& source, TextureTarget target, TextureWrapMode wrapMode,
									TextureFilterMode filterMode);

		~Texture();

		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		Texture(Texture&& other);

		void useTexture(GLuint unit) const;

		GLuint getName() const;
		uint32_t getWidth() const;
		uint32_t getHeight() const;

};

