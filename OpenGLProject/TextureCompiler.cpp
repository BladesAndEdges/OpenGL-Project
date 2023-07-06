#include "TextureCompiler.h"

#include <assert.h>
#include "DDSFileLoader.h"
#include "stb_image.h"

#include "FormatInfo.h"

// --------------------------------------------------------------------------------
TextureCompiler::TextureCompiler()
{
}

// --------------------------------------------------------------------------------
uint32_t TextureCompiler::getVersionNumber() const
{
	return 0u;
}

// --------------------------------------------------------------------------------
void TextureCompiler::compile(ResourceCompilationContext & ioContext) const
{
	const std::string fileExtension = ioContext.getExtension();
	assert(fileExtension.size() > 0u);

	int width, height, numberOfChannels;
	uint8_t* bytes = nullptr;

	if (fileExtension == "dds")
	{
		//DDSFileLoader ddsFileLoader;
		//bytes = nullptr;
	}
	else
	{
		bytes = stbi_load(ioContext.getFilePath().c_str(), &width, &height, &numberOfChannels, 0u);

		const TextureFormat formats[] = { TextureFormat::R8, TextureFormat::RG8, TextureFormat::RGB8, TextureFormat::RGBA8 };
		const uint32_t format = (uint32_t)(formats[numberOfChannels]);

		ioContext.writeUint32(0u);
		ioContext.writeUint32(width);
		ioContext.writeUint32(height);
		ioContext.writeUint32(format);
		ioContext.writeByteArray(bytes, width *  height * numberOfChannels);

		stbi_image_free(bytes);
	}
}

