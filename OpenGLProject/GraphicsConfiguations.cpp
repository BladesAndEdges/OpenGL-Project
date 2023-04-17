#include "GraphicsConfiguations.h"

#include "imgui.h"

// Default Settings: 
// RendererType -> Forward Renderer
// Display ImGui Demo Window -> false
// Shadow Map Dimension Index set to 3 (1024 x 1024)

// --------------------------------------------------------------------------------
GraphicsConfiguations::GraphicsConfiguations() : m_rendererType(RendererType::Forward), 
													m_displayImGuiDemoWindow(false),
													m_shadowMapDimensionsId(3u)
											
{
}

// --------------------------------------------------------------------------------
void GraphicsConfiguations::update()
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
}

// --------------------------------------------------------------------------------
RendererType GraphicsConfiguations::getRendererType() const
{
	return m_rendererType;
}

// --------------------------------------------------------------------------------
uint8_t GraphicsConfiguations::getShadowMapDimensionsId() const
{
	return m_shadowMapDimensionsId;
}
