#include "Texture.h"
#include <experimental/filesystem>
#include <fstream>

#define TEXTURE_MAX_ANISOTROPY_EXT 0x84FE

// --------------------------------------------------------------------------------
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

	GLint numberOfChannels = -1;
	std::vector<GLubyte> textureStorage;
	loadTextureFromDisk(source.c_str(), /* out */ numberOfChannels, /* out */ textureStorage);

	assert((m_width > 0u) && (m_height > 0u));

	TextureFormat format = chooseTextureSizedFormat(numberOfChannels);

	GLenum glSizedFormat = translateFormatToOpenGLSizedFormat(format);
	GLenum glInternalFormat = translateFormatToOpenGLInternalFormat(format);

	if (!textureStorage.empty())
	{
		const uint32_t smallestSide = m_width > m_height ? m_height : m_width;

		const uint32_t mipCount = (uint32_t)ceilf(log2f((float)smallestSide));

		glTextureStorage2D(m_name, mipCount, glSizedFormat, m_width, m_height);
		glTextureSubImage2D(m_name, 0, 0, 0, m_width, m_height, glInternalFormat, GL_UNSIGNED_BYTE, textureStorage.data()); 
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
	glTextureParameterf(m_name, TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
}

// --------------------------------------------------------------------------------
Texture::Texture(const std::string& label, uint32_t width, uint32_t height, uint32_t depth, TextureTarget target,
	TextureWrapMode wrapMode, TextureFilterMode filterMode, TextureFormat format, TextureComparisonMode comparisonMode)
{
	assert(width > 0);
	assert(height > 0);
	assert(depth > 0);

	const GLenum glTarget = translateTargetToOpenGL(target);
	const GLenum glWrapMode = translateWrapModeToOpenGL(wrapMode);
	const GLenum glMinFilterMode = translateFilterModeToOpenGLMinFilter(filterMode); // Min Filter may need to be GL_NEAREST FOR SOME CASES
	const GLenum glMagFilterMode = translateFilterModeToOpenGLMagFilter(filterMode);
	const GLenum glSizedFormat = translateFormatToOpenGLSizedFormat(format);

	glCreateTextures(glTarget, 1, &m_name);
	assert(m_name > 0);

	m_width = width;
	m_height = height;
	m_depth = depth;



	if (glTarget == GL_TEXTURE_2D_ARRAY) 
	{
		glTextureStorage3D(m_name, 1, glSizedFormat, m_width, m_height, m_depth);
	}
	else
	{
		assert(glTarget == GL_TEXTURE_2D);
		glTextureStorage2D(m_name, 1, glSizedFormat, m_width, m_height);
	}

	glObjectLabel(GL_TEXTURE, m_name, -1, label.c_str());

	glTextureParameteri(m_name, GL_TEXTURE_WRAP_S, glWrapMode);
	glTextureParameteri(m_name, GL_TEXTURE_WRAP_T, glWrapMode);
	glTextureParameteri(m_name, GL_TEXTURE_MIN_FILTER, glMinFilterMode);
	glTextureParameteri(m_name, GL_TEXTURE_MAG_FILTER, glMagFilterMode);

	if (comparisonMode != TextureComparisonMode::None)
	{
		glTextureParameteri(m_name, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTextureParameteri(m_name, GL_TEXTURE_COMPARE_FUNC, translateComparisonModeToOpenGL(comparisonMode));
	}
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
void Texture::useTexture(GLuint textureUnit) const
{
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
void Texture::loadTextureFromDisk(const char * sourceFile, GLint& o_numberOfChannels, std::vector<GLubyte>& o_vectorStorage)
{
	const std::string compiledExtension = ".compiled";
	const std::string textureFileName = getFilenameFromTextureSourceString(sourceFile);
	const std::string compiledFileName = textureFileName + compiledExtension;

	GLint width = -1; 
	GLint height = -1;

	tryReadFromCache(compiledFileName.c_str(), width, height, o_numberOfChannels, o_vectorStorage);

	if ((width > 0) && (height > 0))
	{
		m_width = (uint32_t)width;
		m_height = (uint32_t)height;
	}

	if (!o_vectorStorage.data())
	{
		GLubyte* data = stbi_load(sourceFile, &width, &height, &o_numberOfChannels, 0);
		
		const uint32_t allocationInSizeInBytes = width * height * o_numberOfChannels;
		o_vectorStorage.resize(allocationInSizeInBytes);
		memcpy(o_vectorStorage.data(), data, allocationInSizeInBytes);

		if ((width > 0) && (height > 0))
		{
			m_width = (uint32_t)width;
			m_height = (uint32_t)height;
		}

		if (data)
		{
			writeToCache(compiledFileName.c_str(), width, height, (uint32_t)o_numberOfChannels, data); // Since it would be empty for the write
		}

		stbi_image_free(data); // Correct
	}
}

// --------------------------------------------------------------------------------
void Texture::tryReadFromCache(const char* fileName, GLint& width, GLint& height, GLint& numberOfChannels, std::vector<GLubyte>& textureData)
{
	assert(fileName != nullptr);
	assert(textureData.size() == 0);

	const std::string cacheString = "Cache\\";
	const std::string finalPath = cacheString + std::string(fileName);
	assert(finalPath.size() != 0);

	std::ifstream inputStream;
	inputStream.open(finalPath, std::ios::binary);
	if (inputStream.is_open())
	{
		inputStream.read((char*)&(width), sizeof(uint32_t));
		inputStream.read((char*)&(height), sizeof(uint32_t));
		inputStream.read((char*)&(numberOfChannels), sizeof(uint32_t));

		textureData.resize(width * height * numberOfChannels);
		inputStream.read((char*)(textureData.data()), width * height * numberOfChannels);
	}

	// Doesn't fail, it should
}

// --------------------------------------------------------------------------------
void Texture::writeToCache(const char* fileName, uint32_t width, uint32_t height, uint32_t numberOfChannels, GLubyte* data)
{
	assert(fileName != nullptr);
	assert(width > 0u);
	assert(height > 0u);
	assert(numberOfChannels > 0u);

	const std::string directoryName = "Cache";
	const bool directoryCreationSuccess = std::experimental::filesystem::create_directory(directoryName);

	const std::string cacheString = "Cache\\";
	const std::string finalPath = cacheString + std::string(fileName);
	assert(finalPath.size() != 0);

	std::ofstream fw(finalPath, std::ios::binary);
	if (!fw)
	{
		assert(0);
	}

	fw.write((const char*)&(width), sizeof(uint32_t));
	fw.write((const char*)&(height), sizeof(uint32_t));
	fw.write((const char*)&(numberOfChannels), sizeof(uint32_t));
	fw.write((const char*)data, width * height * numberOfChannels); // Magic number for the interal data requirements (GL_RGBA)

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
GLenum Texture::translateFormatToOpenGLSizedFormat(TextureFormat format) const
{
	switch (format)
	{
	case TextureFormat::R8:
		return GL_R8;

	case TextureFormat::RG8:
		return GL_RG8;

	case TextureFormat::RGB8:
		return GL_RGB8;

	case TextureFormat::RGBA8:
		return GL_RGBA8;

	case TextureFormat::DEPTH32:
		return GL_DEPTH_COMPONENT32;

	default:
		assert(false);
		return 0;
	}
}

// --------------------------------------------------------------------------------
GLenum Texture::translateFormatToOpenGLInternalFormat(TextureFormat format) const
{
	switch (format)
	{
	case TextureFormat::R8:
		return GL_RED;

	case TextureFormat::RG8:
		return GL_RG;

	case TextureFormat::RGB8:
		return GL_RGB;

	case TextureFormat::RGBA8:
		return GL_RGBA;

	case TextureFormat::DEPTH32:
		return GL_DEPTH_COMPONENT;

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

// --------------------------------------------------------------------------------
std::string getFilenameFromTextureSourceString(std::string sourceString)
{
	// THIS WILL BREAK FOR ANYTING WITH EXTENSION (with dot) LARGER THAN 4!!!!
	// .png will work, .jpeg will not
	const std::string path = "Meshes\\sponza\\textures\\";
	const uint32_t substringStartPos = (uint32_t)(path.size());
	sourceString.erase(0, substringStartPos);

	const uint32_t sourceStringSize = (uint32_t)sourceString.size();
	const uint32_t substringEndPos = (uint32_t)(sourceStringSize - 3u);
	sourceString.erase(substringEndPos - 1, 4);

	return sourceString;
}
