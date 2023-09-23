#include "GraphicsConfigurations.h"

#include "imgui.h"

#define MAXIMUM_NUM_OF_CASCADES 4

// --------------------------------------------------------------------------------
GraphicsConfigurations::GraphicsConfigurations() : m_rendererType(RendererType::Forward),
													m_displayImGuiDemoWindow(false),
													m_shadowMapDimensionsId(3u),
													m_numberOfAciveCascades(4u),
													m_diffuseLightingEnabled(true),
													m_specularLightingEnabled(true),
													m_normalMappingEnabled(true),
													m_cascadesOverlayModeEnabled(false),
													m_globalLightSourceAzimuth(23.0f),
													m_globalLightSourceZenith(80.0f),
													m_samplingRadiusInTexelUnits(0.0f),
													m_maximumShadowDrawDistance(100.0f),
													m_fadedShadowsStartDistance(0.9f)

{
}

// --------------------------------------------------------------------------------
void GraphicsConfigurations::update()
{
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (m_displayImGuiDemoWindow)
	{
		ImGui::ShowDemoWindow(&m_displayImGuiDemoWindow);
	}

	// Possibly consider making the ui some way docked

	// Update the type of rendering being done
	static ImGuiComboFlags rendererTypeFlags = 0;
	const char* rendererTypeOptions[] = { "Forward", "Deferred" };

	const char* rendererTypePreviewValue = rendererTypeOptions[static_cast<int>(m_rendererType)];
	if (ImGui::BeginCombo("Type of Renderer", rendererTypePreviewValue, rendererTypeFlags))
	{
		for (int id = 0; id < IM_ARRAYSIZE(rendererTypeOptions); id++)
		{
			const bool is_selected = (static_cast<int>(m_rendererType) == id);
			if (ImGui::Selectable(rendererTypeOptions[id], is_selected))
			{
				m_rendererType = static_cast<RendererType>(id);
			}
		}

		ImGui::EndCombo();
	}

	// Update the dimensions of the shadow map being used
	static ImGuiComboFlags shadowMapDimensionFlags = 0;
	const char* shadowMapDimensionOptions[] = { "128x128", "256x256", "512x512", "1024x1024", "2048x2048", "4096x4096" };

	const char* shadowMapDimensionPreviewValue = shadowMapDimensionOptions[m_shadowMapDimensionsId];  // Pass in the preview value visible before opening the combo (it could be anything)

	if (ImGui::BeginCombo("SM Resolution", shadowMapDimensionPreviewValue, shadowMapDimensionFlags))
	{
		for (uint8_t id = 0; id < IM_ARRAYSIZE(shadowMapDimensionOptions); id++)
		{
			const bool is_selected = (m_shadowMapDimensionsId == id);
			if (ImGui::Selectable(shadowMapDimensionOptions[id], is_selected))
			{
				m_shadowMapDimensionsId = (uint8_t)id;
			}
		}

		ImGui::EndCombo();
	}

	// Number of active cascades in use
	char cascadeCountScratch[2];
	cascadeCountScratch[0] = '0' + char(m_numberOfAciveCascades);
	cascadeCountScratch[1] = '\0';

	if (ImGui::BeginCombo("Number Of Cascades", cascadeCountScratch, 0))
	{
		// Begin at 1 as we always want at least one active cascade
		for (uint32_t numberOfCascades = 1u; numberOfCascades <= MAXIMUM_NUM_OF_CASCADES; numberOfCascades++)
		{
			const bool is_selected = (m_numberOfAciveCascades == numberOfCascades);
			cascadeCountScratch[0] = '0' + char(numberOfCascades);

			if (ImGui::Selectable(cascadeCountScratch, is_selected))
			{
				m_numberOfAciveCascades = (uint8_t)numberOfCascades;
			}
		}

		ImGui::EndCombo();
	}

	ImGui::Checkbox("Diffuse Lighting", &m_diffuseLightingEnabled);
	ImGui::Checkbox("Specular", &m_specularLightingEnabled);
	ImGui::Checkbox("Normal Mapping", &m_normalMappingEnabled);
	ImGui::Checkbox("Cascades Overlay", &m_cascadesOverlayModeEnabled);

	ImGui::SliderFloat("Azimuth", &m_globalLightSourceAzimuth, 0.0f, 360.0f);
	ImGui::SliderFloat("Zenith", &m_globalLightSourceZenith, 0.0f, 90.0f);

	ImGui::SliderFloat("PCF Texel Radius", &m_samplingRadiusInTexelUnits, 0.0f, 100.0f);
	ImGui::SliderFloat("Shadow Draw Distance", &m_maximumShadowDrawDistance, 1.0f, 200.0f);
	ImGui::SliderFloat("Shadow Fade Start", &m_fadedShadowsStartDistance, 0.0f, 1.0f);
}

// --------------------------------------------------------------------------------
RendererType GraphicsConfigurations::getRendererType() const
{
	return m_rendererType;
}

// --------------------------------------------------------------------------------
uint8_t GraphicsConfigurations::getShadowMapDimensionsId() const
{
	return m_shadowMapDimensionsId;
}

// --------------------------------------------------------------------------------
uint8_t GraphicsConfigurations::getNumberOfActiveCascades() const
{
	return m_numberOfAciveCascades;
}

// --------------------------------------------------------------------------------
bool GraphicsConfigurations::getDiffuseLightingEnabled() const
{
	return m_diffuseLightingEnabled;
}

// --------------------------------------------------------------------------------
bool GraphicsConfigurations::getSpecularLightingEnabled() const
{
	return m_specularLightingEnabled;
}

// --------------------------------------------------------------------------------
bool GraphicsConfigurations::getNormalMappingEnabled() const
{
	return m_normalMappingEnabled;
}

// --------------------------------------------------------------------------------
bool GraphicsConfigurations::getCascadesOverlayModeEnabled() const
{
	return m_cascadesOverlayModeEnabled;
}

// --------------------------------------------------------------------------------
float GraphicsConfigurations::getGlobalLightSourceAzimuth() const
{
	return m_globalLightSourceAzimuth;
}

// --------------------------------------------------------------------------------
float GraphicsConfigurations::getGlobalLightSourceZenith() const
{
	return m_globalLightSourceZenith;
}

// --------------------------------------------------------------------------------
float GraphicsConfigurations::getSamplingRadiusInTexelUnits() const
{
	return m_samplingRadiusInTexelUnits;
}

// --------------------------------------------------------------------------------
float GraphicsConfigurations::getMaximumShadowDrawDistance() const
{
	return m_maximumShadowDrawDistance;
}

// --------------------------------------------------------------------------------
float GraphicsConfigurations::getFadedShadowsStartDistance() const
{
	return m_fadedShadowsStartDistance;
}

