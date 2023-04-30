#pragma once

// --------------------------------------------------------------------------------
struct PerMaterialUniformData
{
	float m_ambientColour[3]; // 12 bytes
	float _padding0_;	// 4
	float m_diffuseColour[3]; // 12 
	float _padding1_; // 4
	float m_specularColour[3]; // 12
	//float _padding2_ = 1.0f; // 4 // Need explanation for this
	float m_specularHighlight; // 4
}; // 54