#include "TextureCompiler.h"

#include <assert.h>
#include "stb_image.h"

#include "FormatInfo.h"
#include "DDSFileLoader.h"

// --------------------------------------------------------------------------------
TextureCompiler::TextureCompiler() : m_version(0u)
{
}

// --------------------------------------------------------------------------------
uint32_t TextureCompiler::getVersionNumber() const
{
	return m_version;
}

// --------------------------------------------------------------------------------
void TextureCompiler::compile(ResourceCompilationContext & ioContext) const
{
	const std::string fileExtension = ioContext.getExtension();
	assert(fileExtension.size() > 0u);

	int width = -1;
	int height = 1;
	uint32_t dataSizeInBytes = 0u;
	uint8_t* bytes = nullptr;
	uint32_t format = UINT32_MAX;

	if (fileExtension == "dds")
	{
		DDSFileLoader ddsLoader;
		ddsLoader.parse(ioContext.getPath().c_str(), width, height);

		dataSizeInBytes = ddsLoader.getAllocationSizeInBytes();
		bytes = ddsLoader.getTextureData();

		std::vector<uint8_t> blah;
		blah.resize(dataSizeInBytes);

		format = (uint32_t)(ddsLoader.getTextureFormat());
	}
	else
	{
		const TextureFormat eightBitFormats[] = { TextureFormat::R8, TextureFormat::RG8, TextureFormat::RGB8, TextureFormat::RGBA8 };
		int numberOfChannels = -1;

		bytes = stbi_load(ioContext.getPath().c_str(), &width, &height, &numberOfChannels, 0u);

		assert(numberOfChannels > 0u);

		format = (uint32_t)(eightBitFormats[numberOfChannels - 1u]);
		dataSizeInBytes = uint32_t(width) * uint32_t(height) * uint32_t(numberOfChannels);

	}

	assert(width > 0u);
	assert(height > 0u);
	assert(format < UINT32_MAX);
	assert(dataSizeInBytes > 0u);
	assert(bytes != nullptr);

	ioContext.writeUint32(getVersionNumber());
	ioContext.writeUint32((uint32_t)width);
	ioContext.writeUint32((uint32_t)height);
	ioContext.writeUint32(format);
	ioContext.writeUint32(dataSizeInBytes);
	ioContext.writeByteArray(bytes, dataSizeInBytes);

	stbi_image_free(bytes);
}

