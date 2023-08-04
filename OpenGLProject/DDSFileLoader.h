#pragma once

#include<stdint.h>
#include "FormatInfo.h"

struct BlockCompressionInfo;
struct DDSHeader;
struct DDSPixelFormat;

class DDSFileLoader
{
public:

	// Perhaps a try load ??
	void parse(const char* sourceFile, int& width, int& height);

	unsigned char* getTextureData() const;
	uint32_t getAllocationSizeInBytes() const;
	TextureFormat getTextureFormat() const;
		
private:

	unsigned char* m_data;
	TextureFormat m_format;
	uint32_t m_allocationSizeInBytes;
};

BlockCompressionInfo getBlockCompressionInfo(const DDSPixelFormat& ddsPixelFormat);
TextureFormat chooseTextureFormat(const DDSPixelFormat& ddsPixelFormat);

