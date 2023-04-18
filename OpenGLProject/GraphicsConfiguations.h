#pragma once

#include <stdint.h>

// --------------------------------------------------------------------------------
enum class RendererType
{
	Forward,
	Deferred
};

// --------------------------------------------------------------------------------
class GraphicsConfiguations
{
public:

	GraphicsConfiguations();

	void update();

	RendererType getRendererType() const;

	uint8_t getShadowMapDimensionsId() const;
	uint8_t getNumberOfActiveCascades() const;

private:

	RendererType m_rendererType;
	bool m_displayImGuiDemoWindow = false;
	uint8_t m_shadowMapDimensionsId = 3u;
	uint8_t m_numberOfAciveCascades = 4u;

};

