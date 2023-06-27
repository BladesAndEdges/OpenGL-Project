#pragma once

#include "FormatInfo.h"
#include <glad.h>
#include <stdint.h>

#define array_size(x) sizeof(x) / sizeof(x[0u])

#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT    0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT   0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT   0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT   0x83F3

// Format this properly
// --------------------------------------------------------------------------------
const FormatInfo g_glFormatInfoTable[] =
{
	// Width // Height // BLOCK BYTES // BASE FORMAT // SIZED FORMAT										  // COMPRESSED?
	1u,		 1u,	   1u,			  GL_RED,		 GL_R8,													  false,
	1u,      1u,       2u,            GL_RG,		 GL_RG8,                                                  false,
	1u,      1u,       3u,            GL_RGB,        GL_RGB8,				                                  false,
	1u,		 1u,	   4u,			  GL_RGBA,       GL_RGBA8,                                                false,
	1u,		 1u,	   4u,			  GL_RED,	     GL_R32F,                                                 false,
	1u,		 1u,       12u,			  GL_RGB,        GL_RGB32F,                                               false,
	4u,		 4u,	   8u,			  GL_RGB,        GL_COMPRESSED_RGB_S3TC_DXT1_EXT,						  true,
	4u,	     4u,       8u,			  GL_RGBA,       GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,                        true,
	4u,      4u,       16u,           GL_RGBA,       GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,                        true,
	4u,      4u,       16u,           GL_RGBA,       GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,                        true,
	1u,      1u,       4u,            GL_DEPTH,      GL_DEPTH_COMPONENT32,                                    false
};

FormatInfo getFormatInfo(TextureFormat format)
{
	return g_glFormatInfoTable[(uint32_t)format];
}

static_assert(array_size(g_glFormatInfoTable) == (uint32_t)TextureFormat::COUNT, "Format Info table does not match the amount of members of the FormatInfo enum");