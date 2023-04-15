#include "GraphicsConfiguations.h"

#include "imgui.h"

// Default Settings: 
// RendererType -> Forward Renderer

// --------------------------------------------------------------------------------
GraphicsConfiguations::GraphicsConfiguations() : m_rendererType(RendererType::Forward)
{
}

// --------------------------------------------------------------------------------
void GraphicsConfiguations::update()
{
	// Update the type of rendering being done
	static ImGuiComboFlags rendererTyeFlags = 0;
	const char* rendererTypeOptions[] = { "Forward", "Deferred" };

	const char* renderTypePreviewValue = rendererTypeOptions[static_cast<int>(m_rendererType)];
	if (ImGui::BeginCombo("Type of Renderer", renderTypePreviewValue, rendererTyeFlags))
	{
		for (int n = 0; n < IM_ARRAYSIZE(rendererTypeOptions); n++)
		{
			const bool is_selected = (static_cast<int>(m_rendererType) == n);
			if (ImGui::Selectable(rendererTypeOptions[n], is_selected))
			{
				m_rendererType = static_cast<RendererType>(n);
			}

			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
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
