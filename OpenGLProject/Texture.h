#pragma once

#include "stb_image.h"
#include "glad.h"

#include <cstdint>
#include <string>
#include <assert.h>
#include <vector>

#include "FormatInfo.h" // Include here?

// --------------------------------------------------------------------------------
enum class TextureTarget : uint32_t
{
	Texture2D, 
	ArrayTexture2D
};

// --------------------------------------------------------------------------------
enum class TextureWrapMode : uint32_t
{
	Repeat, 
	ClampEdge
};

// --------------------------------------------------------------------------------
// Inspired by Unity's version on doing this
enum class TextureFilterMode : uint32_t
{
	Point, 
	Bilinear,
	Trilinear
};

// --------------------------------------------------------------------------------
enum class TextureComparisonMode : uint32_t
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

	Texture(const std::string& label, uint32_t width,  uint32_t height, uint32_t depth, TextureTarget target
									, TextureWrapMode wrapMode, TextureFilterMode filterMode, 
										TextureFormat format);


	~Texture();

	Texture(const Texture&) = delete;

	Texture& operator=(const Texture&) = delete;

	Texture(Texture&& other);

	void useTexture(GLuint textureUnit, TextureComparisonMode comparisonMode) const;

	GLuint getName() const;

	uint32_t getWidth() const;

	uint32_t getHeight() const;

private:

	GLuint m_name;
	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_depth;

	void loadTextureFromDisk(const std::string& sourceFile, TextureFormat& format, std::vector<GLubyte>& o_vectorStorage);
	void tryReadFromCache(const std::string& path, uint32_t& compilerVersion, GLint& width, GLint& height, TextureFormat& format, uint32_t& allocationSizeInBytes, 
		std::vector<GLubyte>& data);
	void writeToCache(const std::string& fileName, const std::vector<uint8_t>& contents);

	// --------------------------------------------------------------------------------
	GLenum translateTargetToOpenGL(TextureTarget target) const;
	GLenum translateWrapModeToOpenGL(TextureWrapMode wrapMode) const;
	GLenum translateFilterModeToOpenGLMinFilter(TextureFilterMode filterMode) const;
	GLenum translateFilterModeToOpenGLMagFilter(TextureFilterMode filterMode) const;
	GLenum translateComparisonModeToOpenGL(TextureComparisonMode comparionMode) const;
	TextureFormat chooseTextureSizedFormat(int numberOfChannels) const;
};

std::string getFilenameFromTextureSourceString(const char* sourceString);

