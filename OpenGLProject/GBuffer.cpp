#include "GBuffer.h"

// --------------------------------------------------------------------------------
GBuffer::GBuffer(const uint32_t mainViewWidth, const uint32_t mainViewHeight) : m_width(mainViewWidth),
																				m_height(mainViewHeight),
																				m_framebuffer(Framebuffer::customFramebuffer("GBuffer Framebuffer"))
{
	// Create textures
	m_worldPositionTexture = new Texture("gBufferWorldPosition", m_width, m_height, 1, TextureTarget::Texture2D, TextureWrapMode::ClampEdge, 
		TextureFilterMode::Point, TextureFormat::RGB32F, TextureComparisonMode::None);

	m_worldNormalTexture = new Texture("gBufferWorldNormal", m_width, m_height, 1, TextureTarget::Texture2D, TextureWrapMode::ClampEdge,
		TextureFilterMode::Point, TextureFormat::RGB32F, TextureComparisonMode::None);

	m_diffuseColourTexture = new Texture("gBufferDiffuseColour", m_width, m_height, 1, TextureTarget::Texture2D, TextureWrapMode::ClampEdge,
		TextureFilterMode::Point, TextureFormat::RGB8, TextureComparisonMode::None);

	m_specularColourTexture = new Texture("gBufferSpecularColour", m_width, m_height, 1, TextureTarget::Texture2D, TextureWrapMode::ClampEdge,
		TextureFilterMode::Point, TextureFormat::RGB8, TextureComparisonMode::None);

	m_smoothnessTexture = new Texture("gBufferSmoothness", m_width, m_height, 1, TextureTarget::Texture2D, TextureWrapMode::ClampEdge,
		TextureFilterMode::Point, TextureFormat::R32F, TextureComparisonMode::None);

	m_depthTexture = new Texture("gBufferDepth", m_width, m_height, 1, TextureTarget::Texture2D, TextureWrapMode::ClampEdge,
		TextureFilterMode::Point, TextureFormat::DEPTH32, TextureComparisonMode::LessEqual);

	// Attach to framebuffer
	m_framebuffer.attachTexture(TextureTarget::Texture2D, *m_worldPositionTexture, AttachmentType::ColourAttachment, 0, 0);
	m_framebuffer.attachTexture(TextureTarget::Texture2D, *m_worldNormalTexture, AttachmentType::ColourAttachment, 0, 1);
	m_framebuffer.attachTexture(TextureTarget::Texture2D, *m_diffuseColourTexture, AttachmentType::ColourAttachment, 0, 2);
	m_framebuffer.attachTexture(TextureTarget::Texture2D, *m_specularColourTexture, AttachmentType::ColourAttachment, 0, 3);
	m_framebuffer.attachTexture(TextureTarget::Texture2D, *m_smoothnessTexture, AttachmentType::ColourAttachment, 0, 4);
	m_framebuffer.attachTexture(TextureTarget::Texture2D, *m_depthTexture, AttachmentType::DepthAttachment, 0, 0);

	GLenum status = glCheckNamedFramebufferStatus(m_framebuffer.getName(), GL_FRAMEBUFFER);
	assert(status == GL_FRAMEBUFFER_COMPLETE);
}

// --------------------------------------------------------------------------------
GBuffer::~GBuffer()
{
}

// --------------------------------------------------------------------------------
uint32_t GBuffer::getWidth() const
{
	return m_width;
}

// --------------------------------------------------------------------------------
uint32_t GBuffer::getHeight() const
{
	return m_height;
}

// --------------------------------------------------------------------------------
const Framebuffer & GBuffer::getFramebuffer() const
{
	return m_framebuffer;
}

// --------------------------------------------------------------------------------
Texture * GBuffer::getWorldPositionTexture() const
{
	return m_worldPositionTexture;
}

// --------------------------------------------------------------------------------
Texture * GBuffer::getWorldNormalTexture() const
{
	return m_worldNormalTexture;
}

Texture * GBuffer::getDiffuseColourTexture() const
{
	return m_diffuseColourTexture;
}

Texture * GBuffer::getSpecularColourTexture() const
{
	return m_specularColourTexture;
}

Texture * GBuffer::getSmoothnessTexture() const
{
	return m_smoothnessTexture;
}
