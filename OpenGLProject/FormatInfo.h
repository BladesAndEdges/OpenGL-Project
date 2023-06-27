#pragma once

#include <glad.h>
#include <stdint.h>

// --------------------------------------------------------------------------------
enum class TextureFormat : uint32_t
{
	// Unsigned 
	R8,
	RG8,
	RGB8,
	RGBA8,

	// Floating point
	R32F,
	RGB32F,

	// Compressed
	DXT1,
	DXT1A,
	DXT3,
	DXT5,

	// Depth
	DEPTH32,

	COUNT
};

// --------------------------------------------------------------------------------
struct FormatInfo
{
	uint32_t m_blockWidth;
	uint32_t m_blockHeight;
	uint32_t m_bytesInBlock;
	GLenum baseInternalFormat;
	GLenum sizedFormat;
	bool isCompressed;
};

FormatInfo getFormatInfo(TextureFormat format);