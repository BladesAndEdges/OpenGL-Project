#include "DDSFileLoader.h"
#include <windows.h>
#include <assert.h>
#include <fstream>
#include <cstring>
#include <dxgiformat.h>
#include <Mmsystem.h>

// --------------------------------------------------------------------------------
struct BlockCompressionInfo
{
	uint8_t blockSizeInBytes;
	uint8_t blockWidth;
	uint8_t blockHeight;
};

// --------------------------------------------------------------------------------
struct DDSPixelFormat
{
	uint32_t dwSize;
	uint32_t dwFlags;
	uint32_t dwFourCC;
	uint32_t dwRGBBitCount;
	uint32_t dwRBitMask;
	uint32_t dwGBitMask;
	uint32_t dwBBitMask;
	uint32_t dwABitMask;
};

// --------------------------------------------------------------------------------
struct DDSHeader
{
	uint32_t dwSize;
	uint32_t dwFlags;
	uint32_t dwHeight;
	uint32_t dwWidth;
	uint32_t dwPitchOrLinearSize;
	uint32_t dwDepth;
	uint32_t dwMipMapCount;
	uint32_t dwReserved1[11];
	DDSPixelFormat ddspf;
	uint32_t dwCaps;
	uint32_t dwCaps2;
	uint32_t dwCaps3;
	uint32_t dwCaps4;
	uint32_t dwReserved2;
};

// --------------------------------------------------------------------------------
void DDSFileLoader::parse(const char* sourceFile, int& width, int& height)
{
	assert(sourceFile != nullptr);

	std::ifstream ifs;
	ifs.open(sourceFile, std::ios::binary);

	if (ifs.is_open())
	{
		uint32_t magicValue;
		ifs.read((char*)&magicValue, sizeof(uint32_t));

		if (magicValue == 0x20534444)
		{
			DDSHeader ddsHeader;
			ifs.read((char*)&ddsHeader, sizeof(DDSHeader));

			// Set width and height
			width = (int)ddsHeader.dwWidth;
			height = (int)ddsHeader.dwHeight;

			const char formatAsChar[4] = { 'D', 'X', '1', '0' };
			uint32_t formatAsUint;
			memcpy_s(&formatAsUint, sizeof(uint32_t), formatAsChar, sizeof(formatAsChar));


			if ((ddsHeader.ddspf.dwFlags == 0x4) && (ddsHeader.ddspf.dwFourCC == formatAsUint))
			{
				assert(0);
			}

			BlockCompressionInfo blockInfo = getBlockCompressionInfo(ddsHeader.ddspf);
			const uint32_t rowPitch = max(1u, ((ddsHeader.dwWidth + 3u) / 4u)) * blockInfo.blockSizeInBytes;
			const uint32_t blocksAlongY = ((ddsHeader.dwHeight + 3u) / 4u);

			m_allocationSizeInBytes = rowPitch * blocksAlongY;
			m_data = new unsigned char[m_allocationSizeInBytes];
			ifs.read((char*)m_data, m_allocationSizeInBytes);
		}
		else
		{
			// Not DDS
		}
	}
}

// --------------------------------------------------------------------------------
unsigned char * DDSFileLoader::getTextureData() const
{
	return m_data;
}

// --------------------------------------------------------------------------------
uint32_t DDSFileLoader::getAllocationSizeInBytes() const
{
	return m_allocationSizeInBytes;
}

// --------------------------------------------------------------------------------
BlockCompressionInfo getBlockCompressionInfo(const DDSPixelFormat& ddsPixelFormat)
{
	BlockCompressionInfo blockInfo;
	blockInfo.blockSizeInBytes = 0u;
	blockInfo.blockWidth = 0u;
	blockInfo.blockHeight = 0u;

	if (ddsPixelFormat.dwFlags == 0x4) // Compressed
	{	

		if (ddsPixelFormat.dwFourCC == MAKEFOURCC('D', 'X', 'T', '1'))
		{
			blockInfo.blockSizeInBytes = 8u;
			blockInfo.blockWidth = 4u;
			blockInfo.blockHeight = 4u;
		}
		if (ddsPixelFormat.dwFourCC == MAKEFOURCC('D', 'X', 'T', '3'))
		{
			blockInfo.blockSizeInBytes = 16u;
			blockInfo.blockWidth = 4u;
			blockInfo.blockHeight = 4u;
		}

		if (ddsPixelFormat.dwFourCC == MAKEFOURCC('D', 'X', 'T', '5'))
		{
			blockInfo.blockSizeInBytes = 16u;
			blockInfo.blockWidth = 4u;
			blockInfo.blockHeight = 4u;
		}
	}

	assert(blockInfo.blockSizeInBytes > 0u);
	return blockInfo;
}
