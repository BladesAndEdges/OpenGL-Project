#pragma once

#include "Framebuffer.h"

class Texture;

// --------------------------------------------------------------------------------
class GBuffer
{
public:

	GBuffer(const uint32_t screenWidth, const uint32_t screenHeight);
	~GBuffer();

	GBuffer(const GBuffer&) = delete;
	GBuffer& operator=(const GBuffer&) = delete;

	const Framebuffer& getFramebuffer() const;

	Texture* getWorldPositionTexture() const;

private:

	Framebuffer m_framebuffer;

	Texture* m_worldPositionTexture;
	Texture* m_worldNormalTexture;
	Texture* m_diffuseColourTexture;
	Texture* m_specularColourTexture;
	Texture* m_smoothnessTexture;
	Texture* m_depthTexture;
};

