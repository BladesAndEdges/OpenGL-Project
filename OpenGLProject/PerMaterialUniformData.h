#pragma once

// --------------------------------------------------------------------------------
struct PerMaterialUniformData
{
	float m_ambientColour[3];
	float padding0;
	float m_diffuseColour[3];
	float padding1;
	float m_specularColour[3];
	float padding2;
	float m_specularHighlight;
};