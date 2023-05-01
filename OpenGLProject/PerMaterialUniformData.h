#pragma once

// --------------------------------------------------------------------------------
struct PerMaterialUniformData
{
	float m_ambientColour[3]; 
	float _padding0_;
	float m_diffuseColour[3];
	float _padding1_;
	float m_specularColour[3];
	float _padding2_;
	float m_specularHighlight;
	float _padding3_[3];
};