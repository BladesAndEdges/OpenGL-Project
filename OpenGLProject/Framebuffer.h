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

	static Framebuffer defaultFramebuffer();
	static Framebuffer customFramebuffer();
	 
	Framebuffer(const Framebuffer&) = delete;
	Framebuffer& operator=(const Framebuffer&) = delete;

	Framebuffer(Framebuffer&& other);

	~Framebuffer();


	void attachTexture(TextureTarget target, const Texture& texture, const AttachmentType& attachmentType, uint32_t layer);

	GLuint getName() const;

private:

	Framebuffer();

	GLuint m_name;

	GLenum translateAttachmentTypeToOpenGLAttachment(const AttachmentType& attachmentType) const;
};

// --------------------------------------------------------------------------------
void checkFramebufferStatus(GLenum status);

