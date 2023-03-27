#include "Framebuffer.h"

// --------------------------------------------------------------------------------
Framebuffer Framebuffer::defaultFramebuffer()
{
	Framebuffer defaultFramebuffer;
	defaultFramebuffer.m_name = 0;
	// glObjectLabel(GL_FRAMEBUFFER, defaultFramebuffer.m_name, -1, "Default Framebuffer");

	return defaultFramebuffer;
}

// --------------------------------------------------------------------------------
Framebuffer Framebuffer::customFramebuffer(const char* framebufferLabel)
{
	Framebuffer customFramebuffer;

	glCreateFramebuffers(1, &customFramebuffer.m_name);

	if (framebufferLabel != nullptr)
	{
		glObjectLabel(GL_FRAMEBUFFER, customFramebuffer.m_name, -1, framebufferLabel);
	}

	glNamedFramebufferDrawBuffer(customFramebuffer.m_name, GL_NONE);

	glNamedFramebufferReadBuffer(customFramebuffer.m_name, GL_NONE);

	return customFramebuffer;
}

// --------------------------------------------------------------------------------
Framebuffer::Framebuffer(Framebuffer && other) 
	: m_name(other.m_name)
{
	other.m_name = 0;
}

// --------------------------------------------------------------------------------
Framebuffer::~Framebuffer()
{
	if (m_name != 0)
	{
		glDeleteFramebuffers(1, &m_name);
		m_name = 0;
	}
}

// --------------------------------------------------------------------------------
void Framebuffer::attachTexture(TextureTarget target, const Texture & texture, const AttachmentType & attachmentType, uint32_t layer, uint8_t colourAttachmentOffset)
{	
	GLenum glAttachmentType = translateAttachmentTypeToOpenGLAttachment(attachmentType);

	if (target == TextureTarget::Texture2D)
	{
		// Need to check if this works properly for depth attchment
		glNamedFramebufferTexture(m_name, glAttachmentType + colourAttachmentOffset, texture.getName(), 0);
	}
	else if (target == TextureTarget::ArrayTexture2D)
	{
		glNamedFramebufferTextureLayer(m_name, glAttachmentType, texture.getName(), 0, layer);
	}
}

// --------------------------------------------------------------------------------
GLuint Framebuffer::getName() const
{
	return m_name;
}

// --------------------------------------------------------------------------------
Framebuffer::Framebuffer()
{
}

// --------------------------------------------------------------------------------
GLenum Framebuffer::translateAttachmentTypeToOpenGLAttachment(const AttachmentType & attachmentType) const
{
	switch (attachmentType)
	{
	case AttachmentType::ColourAttachment:
		return GL_COLOR_ATTACHMENT0;
		// May need to add more
	case AttachmentType::DepthAttachment:
		return GL_DEPTH_ATTACHMENT;

	default:
		assert(false);
		return 0;
	}
}

// --------------------------------------------------------------------------------
void checkFramebufferStatus(GLenum status)
{
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		std::cout << "GL_FRAMEBUFFER_COMPLETE" << std::endl;
		break;
	case GL_FRAMEBUFFER_UNDEFINED:
		std::cout << "GL_FRAMEBUFFER_UNDEFINED" << std::endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << std::endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << std::endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		std::cout << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" << std::endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		std::cout << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" << std::endl;
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		std::cout << "GL_FRAMEBUFFER_UNSUPPORTED" << std::endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" << std::endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" << std::endl;
		break;
	}
}
