#version 450 core

in vec3 v2f_worldPosition;
in vec2 v2f_textureCoordinate;
in vec3 v2f_objectSpaceNormal;
in vec4 v2f_tangentVector;

layout(binding = 0) uniform sampler2D ambientTextureSampler;
layout(binding = 1) uniform sampler2D baseColourTextureSampler;
layout(binding = 2) uniform sampler2D metalnessTextureSampler;
layout(binding = 3) uniform sampler2D normalMapTextureSampler;
layout(binding = 4) uniform sampler2D maskTextureSampler;

// --------------------------------------------------------------------------------
layout(std140, binding = 7) uniform PerMaterialUniforms
{
	vec4 m_ambientColour;
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
vec3 computeWorldNormal()
{
	const vec4 tangentSpaceNormal = (texture(normalMapTextureSampler,v2f_textureCoordinate) * 2.0f) - 1.0f;
	const vec3 bitangent = v2f_tangentVector.w *  cross(v2f_objectSpaceNormal, vec3(v2f_tangentVector.xyz));
	return normalize(tangentSpaceNormal.x * v2f_tangentVector.xyz + tangentSpaceNormal.y * bitangent + tangentSpaceNormal.z * v2f_objectSpaceNormal);
};

// --------------------------------------------------------------------------------
vec3 getDiffuseColour()
{
	const vec3 baseColour = texture(baseColourTextureSampler, v2f_textureCoordinate).rgb;
	const float metalness = texture(metalnessTextureSampler, v2f_textureCoordinate).r;
	return (1.0f - metalness) * baseColour;
};

// --------------------------------------------------------------------------------
vec3 getSpecularColour()
{
	const vec3 baseColour = texture(baseColourTextureSampler, v2f_textureCoordinate).rgb;
	const float metalness = texture(metalnessTextureSampler, v2f_textureCoordinate).r;
	return metalness * baseColour + (1.0f - metalness) * vec3(0.04f);
};

// --------------------------------------------------------------------------------
SurfaceProperties getSurfaceProperties()
{	
	SurfaceProperties surfaceProperties;
	
	// Position
	surfaceProperties.m_worldPosition = v2f_worldPosition;
	
	// Normal
	surfaceProperties.m_worldNormal = computeWorldNormal();
	
	// Material properties
	surfaceProperties.m_ambientColour = materialUniformData.m_ambientColour.xyz * texture(ambientTextureSampler, v2f_textureCoordinate).rgb;
	surfaceProperties.m_diffuseColour = getDiffuseColour();
	surfaceProperties.m_specularColour = getSpecularColour();
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