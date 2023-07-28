#include "TextureCompiler.h"

#include <assert.h>
#include "stb_image.h"

#include "FormatInfo.h"

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
	int numberOfChannels = -1;
	uint32_t dataSizeInBytes = 0u;
	uint8_t* bytes = nullptr;

	bytes = stbi_load(ioContext.getPath().c_str(), &width, &height, &numberOfChannels, 0u);
	assert(width > 0u);
	assert(height > 0u);
	assert(numberOfChannels > 0u);
	assert(bytes != nullptr);

	dataSizeInBytes = uint32_t(width) * uint32_t(height) * uint32_t(numberOfChannels);
	assert(dataSizeInBytes > 0u);
	
	// Handle with stbi_failure_reason() ?

	const TextureFormat formats[] = { TextureFormat::R8, TextureFormat::RG8, TextureFormat::RGB8, TextureFormat::RGBA8 };
	const uint32_t format = (uint32_t)(formats[numberOfChannels - 1u]);

	ioContext.writeUint32(getVersionNumber());
	ioContext.writeUint32((uint32_t)width);
	ioContext.writeUint32((uint32_t)height);
	ioContext.writeUint32(format);
	ioContext.writeUint32(dataSizeInBytes);
	ioContext.writeByteArray(bytes, dataSizeInBytes);

	stbi_image_free(bytes);
}

