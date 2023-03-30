#version 450 core

#define MAXIMUM_CASCADES 4

layout(binding = 0) uniform sampler2D gBufferPositionSampler;
layout(binding = 1) uniform sampler2D gBufferNormalSampler;
layout(binding = 2) uniform sampler2D gBufferDiffuseColourSampler;
layout(binding = 3) uniform sampler2D gBufferSpecularColourSampler;
layout(binding = 4) uniform sampler2D gBufferSmoothnessSampler;
layout(binding = 5) uniform sampler2DArrayShadow shadowMap;

// Definitions of helepr structs
// --------------------------------------------------------------------------------
struct SurfaceProperties
{
	vec3 m_worldPosition;
	vec3 m_worldNormal; 
	vec3 m_ambientColour;
	vec3 m_diffuseColour;
	vec3 m_specularColour;
	float m_smoothness;
};

layout(std140) uniform sceneMatrices
{
	vec4 worldCameraPosition; 
	vec4 lightSourceDirection; 
	mat4 model;
	mat4 viewProjection;

	mat4 worldToShadowMapMatrices[MAXIMUM_CASCADES];
	
	vec4 cascadeSplitsEndDistances;
	float offsetScale;
	float shadowDrawDistance;
	float shadowFadeStartDistance;
	
	bool normalMapToggle;
	bool ambientToggle;
	bool diffuseToggle;
	bool specularToggle;
	
	bool cascadeDrawDistanceToggle;
}ubo;

// Phong functions
// --------------------------------------------------------------------------------
vec3 calculateDiffuseTerm(const vec3 surfaceDiffuseColour, const vec3 surfaceWorldNormal)
{
	const vec3 c_lightSourceIntensity = vec3(0.723f, 0.535f, 0.1293f);
	const float c_cosTheta = max(dot(ubo.lightSourceDirection.xyz, surfaceWorldNormal), 0.0f);
	
	return c_lightSourceIntensity * surfaceDiffuseColour * c_cosTheta;
};

// --------------------------------------------------------------------------------
vec3 calculateSpecularTerm(const vec3 surfaceSpecularColour, const vec3 surfaceWorldPosition, const vec3 surfaceWorldNormal, const float surfaceSmoothness)
{
	const vec3 c_lightSourceIntensity = vec3(0.723f, 0.535f, 0.1293f);
	
	const vec3 fragmentToCameraDirection = normalize(ubo.worldCameraPosition.xyz - surfaceWorldPosition);
	const vec3 reflectedVector = normalize(reflect(-ubo.lightSourceDirection.xyz, surfaceWorldNormal));
	const float highlightDistribution = pow(max(dot(fragmentToCameraDirection, reflectedVector), 0.0f), surfaceSmoothness);
		
	return c_lightSourceIntensity * surfaceSpecularColour * highlightDistribution; // Ask if this is a correct term
};

// Shadow Mapping-related functions
// --------------------------------------------------------------------------------
float calculateInShadowRatio()
{
	float inShadowRatio;
	return inShadowRatio;
};

// Core functions
// --------------------------------------------------------------------------------
SurfaceProperties getSurfaceProperties()
{	
	SurfaceProperties surfaceProperties;
	
	surfaceProperties.m_worldPosition = texelFetch(gBufferPositionSampler, ivec2(gl_FragCoord.xy), 0).xyz;
	surfaceProperties.m_worldNormal = texelFetch(gBufferNormalSampler,ivec2(gl_FragCoord.xy), 0).xyz;
	surfaceProperties.m_diffuseColour = texelFetch(gBufferDiffuseColourSampler, ivec2(gl_FragCoord.xy), 0).rgb;
	surfaceProperties.m_specularColour = texelFetch(gBufferSpecularColourSampler, ivec2(gl_FragCoord.xy), 0).rgb;
	surfaceProperties.m_smoothness = texelFetch(gBufferSmoothnessSampler, ivec2(gl_FragCoord.xy), 0).r;
	
	return surfaceProperties;
};

// --------------------------------------------------------------------------------
vec3 calculateLightingAtSurfacePoint(const SurfaceProperties surfaceProperties)
{
	const vec3 diffuseTerm = calculateDiffuseTerm(surfaceProperties.m_diffuseColour, surfaceProperties.m_worldNormal);
	const vec3 specularTerm = calculateSpecularTerm(surfaceProperties.m_specularColour, surfaceProperties.m_worldPosition, 
		surfaceProperties.m_worldNormal, surfaceProperties.m_smoothness);
	const float inShadowRatio = calculateInShadowRatio();
	
	return diffuseTerm + specularTerm;
};


// --------------------------------------------------------------------------------
out vec3 o_fragmentColour;

// --------------------------------------------------------------------------------
void main()
{
	// Sample the textures and get the surface information
	SurfaceProperties surfaceProperties = getSurfaceProperties();
	o_fragmentColour = calculateLightingAtSurfacePoint(surfaceProperties);
};