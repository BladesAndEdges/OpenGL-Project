#pragma once

#include "stb_image.h"
#include "glad.h"

#include <cstdint>
#include <string>
#include <assert.h>

// --------------------------------------------------------------------------------
enum class TextureTarget
{
	Texture2D, 
	ArrayTexture2D
};

// --------------------------------------------------------------------------------
enum class TextureWrapMode
{
	Repeat, 
	ClampEdge
};

// --------------------------------------------------------------------------------
// Inspired by Unity's version on doing this
enum class TextureFilterMode
{
	Point, 
	Bilinear,
	Trilinear
};

// --------------------------------------------------------------------------------
enum class TextureFormat
{
	R8,
	RG8,
	RGB8,
	RGBA8, 
	DEPTH32
};

// --------------------------------------------------------------------------------
enum class TextureComparisonMode
{
	None, 
	LessEqual,
	GreaterEqual,
	Less,
	Greater,
	Equal,
	NotEqual,
	Always,
	Never
};

// --------------------------------------------------------------------------------
class Texture
{

public:

	Texture(const std::string& source, TextureTarget target, TextureWrapMode wrapMode,
		TextureFilterMode filterMode);

	Texture(const std::string& label, uint32_t width,  uint32_t height, uint32_t layers, TextureTarget target
									, TextureWrapMode wrapMode, TextureFilterMode filterMode, 
										TextureFormat format, TextureComparisonMode comparisonMode);


	~Texture();

	Texture(const Texture&) = delete;

	Texture& operator=(const Texture&) = delete;

	Texture(Texture&& other);

	void useTexture(GLuint unit) const;

	GLuint getName() const;

	uint32_t getWidth() const;

	uint32_t getHeight() const;

private:

	GLuint m_name;
	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_depth;

	// --------------------------------------------------------------------------------
	GLenum translateTargetToOpenGL(TextureTarget target) const;
	GLenum translateWrapModeToOpenGL(TextureWrapMode wrapMode) const;
	GLenum translateFilterModeToOpenGLMinFilter(TextureFilterMode filterMode) const;
	GLenum translateFilterModeToOpenGLMagFilter(TextureFilterMode filterMode) const;
	GLenum translateFormatToOpenGLSizedFormat(TextureFormat format) const;
	GLenum translateFormatToOpenGLInternalFormat(TextureFormat format) const;
	GLenum translateComparisonModeToOpenGL(TextureComparisonMode comparionMode) const;

	TextureFormat chooseTextureSizedFormat(int numberOfChannels) const;
};

