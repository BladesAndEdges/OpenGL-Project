#pragma once

#include <iostream>
#include <glad.h>
#include "Texture.h"

// --------------------------------------------------------------------------------
enum class AttachmentType
{
	DepthAttachment
};

// --------------------------------------------------------------------------------
class Framebuffer
{

public:

	Framebuffer();

	~Framebuffer();

	void attachTexture(const Texture& texture, const AttachmentType& attachmentType);

	GLuint getName() const;

private:

	GLuint m_name;

	GLenum translateAttachmentTypeToOpenGLAttachment(const AttachmentType& attachmentType) const;
};

// --------------------------------------------------------------------------------
void checkFramebufferStatus(GLenum status);

