#include "Cascade.h"

// Camera's width, height, orientation and position in world space WILL change. But the near and far plane must be the correct values
Cascade::Cascade(Texture* shadowMap, uint32_t layerId) : m_framebuffer(Framebuffer::customFramebuffer()),
m_cascadeView(Camera::orthographic(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, -100.0f, 100.0f)),
														m_shadowMap(shadowMap),
														m_layerId(layerId)
{
	m_framebuffer.attachTexture(TextureTarget::ArrayTexture2D, *m_shadowMap, AttachmentType::DepthAttachment, layerId);
	GLenum status = glCheckNamedFramebufferStatus(m_framebuffer.getName(), GL_FRAMEBUFFER);
	assert(status == GL_FRAMEBUFFER_COMPLETE);
}

Cascade::~Cascade()
{
}

Cascade::Cascade(Cascade && other)
	: m_framebuffer(std::move(other.m_framebuffer)),
	m_cascadeView(other.m_cascadeView),
	m_shadowMap(other.m_shadowMap), 
	m_layerId(other.m_layerId)
{
	other.m_shadowMap = nullptr;
}

const Framebuffer& Cascade::getFramebuffer() const
{
	return m_framebuffer;
}

Camera & Cascade::getCascadeView() 
{
	return m_cascadeView;
}

Texture * Cascade::getShadowMap() const
{
	return m_shadowMap;
}

