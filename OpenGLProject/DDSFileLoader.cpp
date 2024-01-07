#include <windows.h>
#include "DDSFileLoader.h"
#include <assert.h>
#include <fstream>
#include <cstring>
#include <dxgiformat.h>
#include <Mmsystem.h>

#include <iostream>

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

			if ((ddsHeader.ddspf.dwFlags == 0x4) && (ddsHeader.ddspf.dwFourCC == MAKEFOURCC('D', 'X', '1', '0')))
			{
				assert(0); // ...or read DX10 header, not implemented yet.
			}

			width = (int)ddsHeader.dwWidth;
			height = (int)ddsHeader.dwHeight;

			// Data count
			BlockCompressionInfo blockInfo = getBlockCompressionInfo(ddsHeader.ddspf);
			const uint32_t bytesPerBlockRow = max(1u, ((ddsHeader.dwWidth + 3u) / 4u)) * blockInfo.blockSizeInBytes;
			const uint32_t blocksAlongY = ((ddsHeader.dwHeight + 3u) / 4u);

			m_format = chooseTextureFormat(ddsHeader.ddspf);
			m_allocationSizeInBytes = bytesPerBlockRow * blocksAlongY;

			assert(m_allocationSizeInBytes == ddsHeader.dwPitchOrLinearSize);

			m_data = new unsigned char[m_allocationSizeInBytes];
			ifs.read((char*)m_data, m_allocationSizeInBytes);
		}
		else
		{
			throw std::exception("DDS file contains incorrect magic value in file.");
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
TextureFormat DDSFileLoader::getTextureFormat() const
{
	return m_format;
}

// --------------------------------------------------------------------------------
BlockCompressionInfo getBlockCompressionInfo(const DDSPixelFormat& ddsPixelFormat)
{
	BlockCompressionInfo blockInfo;
	blockInfo.blockSizeInBytes = 0u;
	blockInfo.blockWidth = 0u;
	blockInfo.blockHeight = 0u;

	// Check if it is a compressed format
	const DWORD ddpfFourCC = ddsPixelFormat.dwFlags & 0x4;

	if (ddpfFourCC == 0x4) // Compressed
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

		if (ddsPixelFormat.dwFourCC == MAKEFOURCC('A', 'T', 'I', '2'))
		{
			blockInfo.blockSizeInBytes = 16u;
			blockInfo.blockWidth = 4u;
			blockInfo.blockHeight = 4u;
		}
	}

	assert(blockInfo.blockSizeInBytes > 0u);
	return blockInfo;
}


// --------------------------------------------------------------------------------
TextureFormat chooseTextureFormat(const DDSPixelFormat & ddsPixelFormat)
{
	TextureFormat format = TextureFormat::COUNT;

	const DWORD ddpfFourCC = ddsPixelFormat.dwFlags & 0x4;
	if (ddpfFourCC == 0x4) // Compressed
	{

		if (ddsPixelFormat.dwFourCC == MAKEFOURCC('D', 'X', 'T', '1'))
		{
			format = TextureFormat::DXT1;
		}
		if (ddsPixelFormat.dwFourCC == MAKEFOURCC('D', 'X', 'T', '3'))
		{
			format = TextureFormat::DXT3;
		}
		if (ddsPixelFormat.dwFourCC == MAKEFOURCC('D', 'X', 'T', '5'))
		{
			format = TextureFormat::DXT5;
		}
		if (ddsPixelFormat.dwFourCC == MAKEFOURCC('A', 'T', 'I', '2'))
		{
			format = TextureFormat::ATI2;
		}
	}

	assert(format != TextureFormat::COUNT);
	return format;
}
