#include "Texture.h"
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include "ResourceCompiler.h"
#include "TextureCompiler.h"

#define GL_TEXTURE_MAX_ANISOTROPY_EXT	0x84FE



// -------------------------------------------------------------------------------- // Possibly assumes colour texture
Texture::Texture(const std::string & source, TextureTarget target, TextureWrapMode wrapMode,
	TextureFilterMode filterMode)
{
	const GLenum glTarget = translateTargetToOpenGL(target);
	const GLenum glWrapMode = translateWrapModeToOpenGL(wrapMode);
	const GLenum glMinFilterMode = translateFilterModeToOpenGLMinFilter(filterMode);
	const GLenum glMagFilterMode = translateFilterModeToOpenGLMagFilter(filterMode);
	
	glCreateTextures(glTarget, 1, &m_name);
	assert(m_name > 0);

	stbi_set_flip_vertically_on_load(true);

	TextureFormat format;
	std::vector<GLubyte> textureStorage;
	loadTextureFromDisk(source, /* out */ format, /* out */ textureStorage);
	assert((m_width > 0u) && (m_height > 0u));

	FormatInfo formatInfo = getFormatInfo(format);

	if (!textureStorage.empty())
	{
		const uint32_t smallestSide = m_width > m_height ? m_height : m_width;

		const uint32_t mipCount = (uint32_t)ceilf(log2f((float)smallestSide));
		glTextureStorage2D(m_name, mipCount, formatInfo.sizedFormat, m_width, m_height);
		glTextureSubImage2D(m_name, 0, 0, 0, m_width, m_height, formatInfo.baseInternalFormat, GL_UNSIGNED_BYTE, textureStorage.data()); 
	}
	else
	{
		throw std::exception("Could not load texture image!");
	}

	glObjectLabel(GL_TEXTURE, m_name, -1, source.c_str());

	glGenerateTextureMipmap(m_name);

	glTextureParameteri(m_name, GL_TEXTURE_WRAP_S, glWrapMode);
	glTextureParameteri(m_name, GL_TEXTURE_WRAP_T, glWrapMode);
	glTextureParameteri(m_name, GL_TEXTURE_MIN_FILTER, glMinFilterMode);
	glTextureParameteri(m_name, GL_TEXTURE_MAG_FILTER, glMagFilterMode);
	glTextureParameterf(m_name, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
}

// --------------------------------------------------------------------------------
Texture::Texture(const std::string& label, uint32_t width, uint32_t height, uint32_t depth, TextureTarget target,
	TextureWrapMode wrapMode, TextureFilterMode filterMode, TextureFormat format)
{
	// Note: When TextureTarget::Texture2D is used, depth can be whatever, as it will never be used. Perhaps handle this somehow?
	assert(width > 0);
	assert(height > 0);
	assert(depth > 0);

	const GLenum glTarget = translateTargetToOpenGL(target);
	const GLenum glWrapMode = translateWrapModeToOpenGL(wrapMode);
	const GLenum glMinFilterMode = translateFilterModeToOpenGLMinFilter(filterMode); // Min Filter may need to be GL_NEAREST FOR SOME CASES
	const GLenum glMagFilterMode = translateFilterModeToOpenGLMagFilter(filterMode);
	const FormatInfo formatInfo = getFormatInfo(format);

	glCreateTextures(glTarget, 1, &m_name);
	assert(m_name > 0);

	m_width = width;
	m_height = height;
	m_depth = depth;



	if (glTarget == GL_TEXTURE_2D_ARRAY) 
	{
		glTextureStorage3D(m_name, 1, formatInfo.sizedFormat, m_width, m_height, m_depth);
	}
	else
	{
		assert(glTarget == GL_TEXTURE_2D);
		glTextureStorage2D(m_name, 1, formatInfo.sizedFormat, m_width, m_height);
	}

	glObjectLabel(GL_TEXTURE, m_name, -1, label.c_str());

	glTextureParameteri(m_name, GL_TEXTURE_WRAP_S, glWrapMode);
	glTextureParameteri(m_name, GL_TEXTURE_WRAP_T, glWrapMode);
	glTextureParameteri(m_name, GL_TEXTURE_MIN_FILTER, glMinFilterMode);
	glTextureParameteri(m_name, GL_TEXTURE_MAG_FILTER, glMagFilterMode);
}

// --------------------------------------------------------------------------------
Texture::~Texture()
{
	if (m_name != 0)
	{
		glDeleteTextures(1, &m_name);
		m_name = 0;
	}
}

// --------------------------------------------------------------------------------
Texture::Texture(Texture && other)
	: m_name(other.m_name),
	m_width(other.m_width),
	m_height(other.m_height)
{
	other.m_name = 0;
	other.m_width = 0;
	other.m_height = 0;
}

// --------------------------------------------------------------------------------
void Texture::useTexture(GLuint textureUnit, TextureComparisonMode comparisonMode) const
{
	if (comparisonMode != TextureComparisonMode::None)
	{
		glTextureParameteri(m_name, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTextureParameteri(m_name, GL_TEXTURE_COMPARE_FUNC, translateComparisonModeToOpenGL(comparisonMode));
	}
	else
	{
		glTextureParameteri(m_name, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	}

	glBindTextureUnit(textureUnit, m_name);
}

// --------------------------------------------------------------------------------
GLuint Texture::getName() const
{
	return m_name;
}

// --------------------------------------------------------------------------------
uint32_t Texture::getWidth() const
{
	return m_width;
}

// --------------------------------------------------------------------------------
uint32_t Texture::getHeight() const
{
	return m_height;
}

// --------------------------------------------------------------------------------
void Texture::loadTextureFromDisk(const std::string& sourceFile, TextureFormat& format, std::vector<GLubyte>& o_vectorStorage)
{	
	const std::string cacheString = "Cache\\";
	const std::string compiledFileName = getFilenameFromTextureSourceString(sourceFile.c_str()) + std::string(".compiled");
	const std::string compiledFilePath = cacheString + compiledFileName;

	TextureCompiler compiler;
	//TextureLoader loader;

	// Check if the file is available to be read, if not compile the file
	const bool isAvailable = std::experimental::filesystem::exists(compiledFilePath);
	if (!isAvailable)
	{
		ResourceCompilationContext compilationContext(sourceFile);
		compiler.compile(compilationContext);
		writeToCache(compiledFileName, compilationContext.getContents());
	}

	// Attempt loading the file
	int width, height;
	uint32_t compilerVersion, allocationSizeInBytes;
	tryReadFromCache(compiledFilePath.c_str(), compilerVersion, width, height, format, allocationSizeInBytes, o_vectorStorage);

	if (compiler.getVersionNumber() != compilerVersion /*loader.getCompilationVersion()*/)
	{
		/*std::experimental::filesystem::remove(compiledFilePath);*/

		ResourceCompilationContext compilationContext(sourceFile);
		compiler.compile(compilationContext);
		writeToCache(compiledFilePath.c_str(), compilationContext.getContents());

		// Clear previously loaded data
		width = 0u;
		height = 0u;
		allocationSizeInBytes = 0u;
		o_vectorStorage.clear();

		tryReadFromCache(sourceFile.c_str(), compilerVersion, width, height, format, allocationSizeInBytes, o_vectorStorage);
	}

	// Assign width and height
	m_width = (uint32_t)width;
	m_height = (uint32_t)height;
}

// --------------------------------------------------------------------------------
void Texture::tryReadFromCache(const std::string& path, uint32_t& compilerVersion, GLint& width, GLint& height, TextureFormat& format, uint32_t& allocationSizeInBytes, std::vector<GLubyte>& data)
{
	assert(path.size() != 0);
	assert(data.size() == 0);

	std::ifstream inputStream;
	inputStream.open(path, std::ios::binary);
	if (inputStream.is_open())
	{
		inputStream.read((char*)&(compilerVersion), sizeof(uint32_t));
		inputStream.read((char*)&(width), sizeof(uint32_t));
		inputStream.read((char*)&(height), sizeof(uint32_t));
		inputStream.read((char*)&(format), sizeof(uint32_t));
		inputStream.read((char*)&(allocationSizeInBytes), sizeof(uint32_t));

		data.resize(allocationSizeInBytes);
		inputStream.read((char*)(data.data()), allocationSizeInBytes);
	}

	assert(width > 0u);
	assert(height > 0u);
	assert(allocationSizeInBytes > 0u);
	assert(data.size() > 0u);
}

// --------------------------------------------------------------------------------
void Texture::writeToCache(const std::string& fileName, const std::vector<uint8_t>& contents)
{
	assert(fileName.size() != 0u);
	assert(contents.size() != 0u);

	const std::string directoryName = "Cache";
	const bool directoryCreationSuccess = std::experimental::filesystem::create_directory(directoryName); // May be worth moving to set up code

	const std::string cacheString = "Cache\\";
	const std::string finalPath = cacheString + std::string(fileName);
	assert(finalPath.size() != 0);

	std::ofstream fw(finalPath, std::ios::binary);
	if (!fw)
	{
		assert(0);
	}

	fw.write((const char*)contents.data(), contents.size());
	fw.close();
}

// --------------------------------------------------------------------------------
GLenum Texture::translateTargetToOpenGL(TextureTarget target) const
{
	switch (target)
	{
	case TextureTarget::Texture2D:
		return GL_TEXTURE_2D;
	case TextureTarget::ArrayTexture2D:
		return GL_TEXTURE_2D_ARRAY;
	default:
		assert(false);
		return 0;
	}
}

// --------------------------------------------------------------------------------
GLenum Texture::translateWrapModeToOpenGL(TextureWrapMode wrapMode) const
{
	switch (wrapMode)
	{
	case TextureWrapMode::Repeat:
		return GL_REPEAT;

	case TextureWrapMode::ClampEdge:
		return GL_CLAMP_TO_EDGE;

	default:
		assert(false);
		return 0;
	}
}

// --------------------------------------------------------------------------------
GLenum Texture::translateFilterModeToOpenGLMinFilter(TextureFilterMode filterMode) const
{
	switch (filterMode)
	{
	case TextureFilterMode::Point:
		return GL_NEAREST_MIPMAP_NEAREST;

	case TextureFilterMode::Bilinear:
		return GL_LINEAR_MIPMAP_NEAREST;

	case TextureFilterMode::Trilinear:
		return GL_LINEAR_MIPMAP_LINEAR;

	default:
		assert(false);
		return 0;
	}
}

// --------------------------------------------------------------------------------
GLenum Texture::translateFilterModeToOpenGLMagFilter(TextureFilterMode filterMode) const
{
	switch (filterMode)
	{
	case TextureFilterMode::Point:
		return GL_NEAREST;

	case TextureFilterMode::Bilinear:
	case TextureFilterMode::Trilinear:
		return GL_LINEAR;

	default:
		assert(false);
		return 0;
	}
}

// --------------------------------------------------------------------------------
GLenum Texture::translateComparisonModeToOpenGL(TextureComparisonMode comparionMode) const
{
	switch (comparionMode)
	{
	case TextureComparisonMode::LessEqual: return GL_LEQUAL;
	case TextureComparisonMode::GreaterEqual: return GL_GEQUAL;
	case TextureComparisonMode::Less: return GL_LESS;
	case TextureComparisonMode::Greater: return GL_GREATER;
	case TextureComparisonMode::Equal: return GL_EQUAL;
	case TextureComparisonMode::NotEqual: return GL_NOTEQUAL;
	case TextureComparisonMode::Always: return GL_ALWAYS;
	case TextureComparisonMode::Never: return GL_NEVER;
	default:
		assert(false);
		return 0;
	}
}

// --------------------------------------------------------------------------------
TextureFormat Texture::chooseTextureSizedFormat(int numberOfChannels) const
{
	const TextureFormat colourFormats[4] =
	{
		TextureFormat::R8,
		TextureFormat::RG8,
		TextureFormat::RGB8,
		TextureFormat::RGBA8,
	};

	const uint32_t index = (uint32_t)(numberOfChannels - 1);

	return colourFormats[index];
}

// Currently a 1-to-1 copy of another function located in ResourceCompiler.cpp
// This function here will be removed as soon as I get a better understanding of how
// I can pair up all the classes into different classes that will interact together.
// --------------------------------------------------------------------------------
std::string getFilenameFromTextureSourceString(const char * sourceString)
{
	std::string fileName(sourceString);

	// Cut directory path
	for (uint32_t charPosition = (uint32_t)fileName.size(); charPosition >= 0u; charPosition--)
	{
		if (fileName[charPosition] == '\\')
		{
			fileName.erase(0, charPosition + 1u);
			break;
		}
	}

	// Cut the extension 
	for (uint32_t charPosition = (uint32_t)fileName.size(); charPosition >= 0u; charPosition--)
	{
		if (fileName[charPosition] == '.')
		{
			const uint32_t countToDelete = (uint32_t)(fileName.size() - charPosition);
			fileName.erase(charPosition, countToDelete);
			break;
		}
	}

	return fileName;
}
