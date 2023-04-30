#version 450 core

in vec3 v2f_worldPosition;
in vec2 v2f_textureCoordinate;
in vec3 v2f_worldNormalBeforeTangent;

layout(binding = 0) uniform sampler2D ambientTextureSampler;
layout(binding = 1) uniform sampler2D diffuseTextureSampler;
layout(binding = 2) uniform sampler2D specularTextureSampler;
layout(binding = 3) uniform sampler2D normalMapTextureSampler;
layout(binding = 4) uniform sampler2D maskTextureSampler;

// --------------------------------------------------------------------------------
layout(std140, binding = 7) uniform PerMaterialUniforms
{
	vec4 m_ambientColour;
	vec4 m_diffuseColour;
	vec4 m_specularColour;
	float m_specularHighLight;
} materialUniformData;

struct SurfaceProperties
{
	vec3 m_worldPosition;
	vec3 m_worldNormal; 
	vec3 m_ambientColour;
	vec3 m_diffuseColour;
	vec3 m_specularColour;
	float m_smoothness;
	float m_opacity;
};

// --------------------------------------------------------------------------------
SurfaceProperties getSurfaceProperties()
{	
	SurfaceProperties surfaceProperties;
	
	// Position
	surfaceProperties.m_worldPosition = v2f_worldPosition;
	
	// Normal
	surfaceProperties.m_worldNormal = v2f_worldNormalBeforeTangent;
	
	// Material properties
	surfaceProperties.m_ambientColour = materialUniformData.m_ambientColour.xyz * texture(ambientTextureSampler, v2f_textureCoordinate).rgb;
	surfaceProperties.m_diffuseColour = materialUniformData.m_diffuseColour.xyz * texture(diffuseTextureSampler, v2f_textureCoordinate).rgb;
	surfaceProperties.m_specularColour = materialUniformData.m_specularColour.xyz * texture(specularTextureSampler, v2f_textureCoordinate).rgb;
	surfaceProperties.m_smoothness = materialUniformData.m_specularHighLight;
	surfaceProperties.m_opacity = texture(maskTextureSampler, v2f_textureCoordinate).r;

	return surfaceProperties;
};

// --------------------------------------------------------------------------------
layout(location = 0) out vec3 out_gWorldPosition;
layout(location = 1) out vec3 out_gWorldNormal;
layout(location = 2) out vec3 out_gDiffuseColour;
layout(location = 3) out vec3 out_gSpecularColour;
layout(location = 4) out vec3 out_gSmoothness;
//layout(location = 5) out float out_gDepth;

// --------------------------------------------------------------------------------
void main()
{
	const float c_alphaTreshold = 0.5f;
	
	// Get the surface properties of the material at the point
	SurfaceProperties surfaceProperties = getSurfaceProperties();
	
	if(surfaceProperties.m_opacity < c_alphaTreshold) discard;
	
	// Output each attribute to the corresponding location
	out_gWorldPosition = surfaceProperties.m_worldPosition;
	out_gWorldNormal = surfaceProperties.m_worldNormal;
	out_gDiffuseColour = surfaceProperties.m_diffuseColour;
	out_gSpecularColour = surfaceProperties.m_specularColour;
	out_gSmoothness = vec3(surfaceProperties.m_smoothness, 0.0f, 0.0f);
}