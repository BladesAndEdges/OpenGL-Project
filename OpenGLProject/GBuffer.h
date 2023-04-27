#pragma once

#include "Framebuffer.h"

class Texture;

// --------------------------------------------------------------------------------
class GBuffer
{
public:

	GBuffer(const uint32_t mainViewWidth, const uint32_t mainViewHeight);
	~GBuffer();

	GBuffer(const GBuffer&) = delete;
	GBuffer& operator=(const GBuffer&) = delete;

	uint32_t getWidth() const;
	uint32_t getHeight() const;

	const Framebuffer& getFramebuffer() const;

	Texture* getWorldPositionTexture() const;
	Texture* getWorldNormalTexture() const;
	Texture* getDiffuseColourTexture() const;
	Texture* getSpecularColourTexture() const;
	Texture* getSmoothnessTexture() const;

private:

	uint32_t m_width;
	uint32_t m_height;

	Framebuffer m_framebuffer;

	Texture* m_worldPositionTexture;
	Texture* m_worldNormalTexture;
	Texture* m_diffuseColourTexture;
	Texture* m_specularColourTexture;
	Texture* m_smoothnessTexture;
	Texture* m_depthTexture;
};

