#pragma once

#include <stdint.h>
#include <string>
#include <vector>

// --------------------------------------------------------------------------------
class ResourceCompilationContext
{
public:

	ResourceCompilationContext(const std::string& filePath);

	const std::string& getFilePath() const;
	const std::string& getExtension() const;
	const std::string& getFileNameSubstring() const;

	void writeUint32(uint32_t value);
	void writeByteArray(const uint8_t* bytes, uint32_t count);

private:

	std::string parseFileNameSubstring(const std::string& filePath);
	std::string parseExtension(const std::string& filePath);

	std::vector<uint8_t> m_resourceData;

	std::string m_filePath;
	std::string m_fileExtension;
	std::string m_fileNameSubstring;
};

// --------------------------------------------------------------------------------
class ResourceCompiler
{
public:
	
	virtual uint32_t getVersionNumber() const = 0;
	virtual void compile(ResourceCompilationContext& resourceInfo) const = 0;

private:

	virtual void setUpCompiledData() const = 0;
};