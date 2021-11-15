#include "Framebuffer.h"

// --------------------------------------------------------------------------------
Framebuffer::Framebuffer()
{
	glCreateFramebuffers(1, &m_name);

	glNamedFramebufferDrawBuffer(m_name, GL_NONE);

	glNamedFramebufferReadBuffer(m_name, GL_NONE);
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
void Framebuffer::attachTexture(const Texture & texture, const AttachmentType & attachmentType)
{
	GLenum glAttachmentType = translateAttachmentTypeToOpenGLAttachment(attachmentType);

	glNamedFramebufferTexture(m_name, glAttachmentType, texture.getName(), 0);
}

// --------------------------------------------------------------------------------
GLuint Framebuffer::getName() const
{
	return m_name;
}

// --------------------------------------------------------------------------------
GLenum Framebuffer::translateAttachmentTypeToOpenGLAttachment(const AttachmentType & attachmentType) const
{
	switch (attachmentType)
	{
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
