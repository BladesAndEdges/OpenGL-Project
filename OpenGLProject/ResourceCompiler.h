#pragma once

#include <stdint.h>
#include <string>
#include <vector>

// --------------------------------------------------------------------------------
class ResourceCompilationContext
{
public:

	ResourceCompilationContext(const std::string& path);

	const std::string& getPath() const;
	const std::string& getExtension() const;
	const std::string& getFileName() const;
	const std::vector<uint8_t>& getContents() const; // return const ref vs non-const ref
	bool isEmpty() const;

	void writeUint32(uint32_t value);
	void writeByteArray(const uint8_t* bytes, uint32_t count);

private:

	std::string parseFileName(const std::string& path);
	std::string parseExtension(const std::string& path);

	std::vector<uint8_t> m_contents;

	std::string m_path;
	std::string m_extension;
	std::string m_fileName;
};

// --------------------------------------------------------------------------------
class ResourceCompiler
{
public:
	
	virtual uint32_t getVersionNumber() const = 0;
	virtual void compile(ResourceCompilationContext& context) const = 0;
};