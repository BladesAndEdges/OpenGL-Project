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

	bool getDiffuseLightingEnabled() const;
	bool getSpecularLightingEnabled() const;
	bool getNormalMappingEnabled() const;
	
	bool getCascadesOverlayModeEnabled() const;

private:

	RendererType m_rendererType;
	bool m_displayImGuiDemoWindow;
	uint8_t m_shadowMapDimensionsId;
	uint8_t m_numberOfAciveCascades;

	bool m_diffuseLightingEnabled;
	bool m_normalMappingEnabled;
	bool m_specularLightingEnabled;

	// Overlays
	bool m_cascadesOverlayModeEnabled;
};

