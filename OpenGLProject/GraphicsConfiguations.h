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

	bool getShadowMapDimensionsChanged() const;
	uint8_t getShadowMapDimensionsId() const;

private:

	RendererType m_rendererType;
	bool m_displayImGuiDemoWindow = false;
	uint8_t m_shadowMapDimensionsId = 3u;

};

