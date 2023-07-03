#include "ResourceCompiler.h"
#include <assert.h>

// --------------------------------------------------------------------------------
ResourceCompilationContext::ResourceCompilationContext(const std::string & filePath) : m_filePath(filePath)
{
	m_fileNameSubstring = parseFileNameSubstring(m_filePath);
	m_fileExtension = parseExtension(m_filePath);
}

// --------------------------------------------------------------------------------
const std::string & ResourceCompilationContext::getFilePath() const
{
	return m_filePath;
}

// --------------------------------------------------------------------------------
const std::string & ResourceCompilationContext::getExtension() const
{
	return m_fileExtension;
}

// --------------------------------------------------------------------------------
const std::string & ResourceCompilationContext::getFileNameSubstring() const
{
	return m_fileNameSubstring;
}

// --------------------------------------------------------------------------------
void ResourceCompilationContext::writeUint32(uint32_t value)
{
	const uint8_t a = (uint8_t)(value >> 24u);
	const uint8_t b = (uint8_t)(value >> 16u);
	const uint8_t c = (uint8_t)(value >> 8u);
	const uint8_t d = (uint8_t)(value);

	m_resourceData.push_back(a);
	m_resourceData.push_back(b);
	m_resourceData.push_back(c);
	m_resourceData.push_back(d);
}

// --------------------------------------------------------------------------------
void ResourceCompilationContext::writeByteArray(const uint8_t * bytes, uint32_t count)
{
	for (uint32_t index = 0u; index < count; index++)
	{
		m_resourceData.push_back(bytes[index]);
	}
}

// --------------------------------------------------------------------------------
std::string ResourceCompilationContext::parseFileNameSubstring(const std::string & filePath)
{
	assert(filePath.size() > 0u);

	std::string fileName = filePath;

	// Cut directory path
	for (uint32_t charPosition = (uint32_t)fileName.size() - 1u; charPosition >= 0u; charPosition--)
	{
		if (fileName[charPosition] == '\\')
		{
			fileName.erase(0, charPosition + 1u);
			break;
		}
	}

	// Cut the extension 
	for (uint32_t charPosition = (uint32_t)fileName.size() - 1u; charPosition >= 0u; charPosition--)
	{
		if (fileName[charPosition] == '.')
		{
			const uint32_t countToDelete = (uint32_t)(fileName.size() - charPosition);
			fileName.erase(charPosition, countToDelete);
			break;
		}
	}

	assert(fileName.size() > 0u);

	return fileName;
}

// --------------------------------------------------------------------------------
std::string ResourceCompilationContext::parseExtension(const std::string & filePath)
{
	assert(filePath.size() > 0u);

	std::string extension = filePath;

	for (uint32_t charPosition = (uint32_t)extension.size() - 1u; charPosition >= 0u; charPosition--)
	{
		if (extension[charPosition] == '.')
		{
			extension.erase(0u, charPosition + 1u);
		}
	}

	assert(extension.size() > 0u);

	return extension;
}
