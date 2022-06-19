#pragma once

#include "Texture.h"
#include "Framebuffer.h"
#include "Camera.h"

class Cascade
{

public:

	Cascade(Texture* shadowMap, uint32_t layerId);
	~Cascade();

	Cascade(const Cascade&) = delete;
	Cascade& operator=(const Cascade&) = delete;

	Cascade(Cascade&& other);

	const Framebuffer& getFramebuffer() const;
	Camera& getCascadeView();
	Texture* getShadowMap() const;

private:

	Framebuffer m_framebuffer;
	Camera m_cascadeView;
	Texture* m_shadowMap;
	uint32_t m_layerId;
};

