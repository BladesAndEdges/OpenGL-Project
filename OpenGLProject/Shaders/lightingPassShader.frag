#version 450 core

#define PI 3.1415926538
#define MAXIMUM_CASCADES 4
#define UINT_MAX 0xFFFFFFFFu

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
	bool padding;
	bool diffuseToggle;
	bool specularToggle;
	
	bool cascadeDrawDistanceToggle;
}ubo;

// Phong functions
// --------------------------------------------------------------------------------
vec3 calculateIndirectDiffuseTerm(const vec3 surfaceDiffuseColour)
{
	const vec3 c_globalLightSourceIntensity = vec3(0.8f, 0.8f, 0.8f);
	return c_globalLightSourceIntensity * surfaceDiffuseColour;
};

// --------------------------------------------------------------------------------
vec3 calculateDirectDiffuseTerm(const vec3 surfaceDiffuseColour, const vec3 surfaceWorldNormal)
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
float interleavedGradientNoise()
{
	vec3 magic = vec3(0.06711056f, 0.00583715f, 52.9829189f);
	return fract(magic.z * fract(dot(gl_FragCoord.xy, magic.xy)));
};

// --------------------------------------------------------------------------------
float computeShadowStrength(float maximumDrawDistance, float fadingStartDistance, float fragmentDistance)
{
	// y = mx + b;
	float slope = (0.0f - 1.0f) / (maximumDrawDistance - fadingStartDistance);
	float b = 1.0f - slope * fadingStartDistance;

	return clamp(slope * fragmentDistance + b, 0.0f, 1.0f);
};

// --------------------------------------------------------------------------------
uint pickCascadeSplit(const float fragmentDistance, const vec4 cascadeSplitsEndDistances)
{
	// Cascades[0] is redundant and should be removed. First cascade always starts at 0.
	// 4th cascade always ends at maxShadowDrawDistance.
	if(fragmentDistance <= cascadeSplitsEndDistances[0])
	{
		return 0;
	}
	else if(fragmentDistance <= cascadeSplitsEndDistances[1])
	{
		return 1;
	}
	else if(fragmentDistance <= cascadeSplitsEndDistances[2])
	{
		return 2;
	}
	else if(fragmentDistance <= cascadeSplitsEndDistances[3])
	{
		return 3;
	}
	else
	{
		return UINT_MAX;
	}
};

// --------------------------------------------------------------------------------
float computeInShadowRatio(float offsetScale, vec3 shadowMapFragment, const vec3 mainCameraWorldPosition, 
											const vec3 fragmentWorldPosition, float maximumShadowDrawDistance, float shadowFadeStartDistance, uint cascadeSplitId)
{	

//#if 0
	const vec2 sampleOffsets[8]=vec2[8](
	vec2(-0.7071, 0.7071),
	vec2(-0.0000, -0.8750),
	vec2(0.5303, 0.5303),
	vec2(-0.6250, -0.0000),
	vec2(0.3536, -0.3536),
	vec2(-0.0000, 0.3750),
	vec2(-0.1768, -0.1768),
	vec2(0.1250, 0.0000)
	);
	
	const vec3 mainCameraToFragment = fragmentWorldPosition - mainCameraWorldPosition;
	const float mainCameraToFragmentMagnitude = length(mainCameraToFragment);
	const bool renderShadows = (mainCameraToFragmentMagnitude <= maximumShadowDrawDistance);

	if(renderShadows)
	{
		vec3 currentFragmentDepthTextureCoords = shadowMapFragment * 0.5f + 0.5f;
		currentFragmentDepthTextureCoords.z -= 0.0005;
		
		float total = 0.0f;
		
		for(uint offset = 0; offset < sampleOffsets.length(); offset++)
		{
			const float rotation = 2.0f * PI * interleavedGradientNoise();
		
			float sineRotation = sin(rotation);
			float cosineRotation = cos(rotation);
		
			const mat2 rotationMatrix = mat2( vec2(cosineRotation, sineRotation), 
											vec2(-sineRotation, cosineRotation));
											
			const vec2 shadowMapOffset = currentFragmentDepthTextureCoords.xy + (rotationMatrix * (offsetScale * sampleOffsets[offset]));
			
			const vec3 shadowTextureCoordinates = vec3(shadowMapOffset, currentFragmentDepthTextureCoords.z);
			total += texture(shadowMap, vec4(vec3(shadowTextureCoordinates.xy, cascadeSplitId), currentFragmentDepthTextureCoords.z));		
		}
		
		float shadowStrength = computeShadowStrength(maximumShadowDrawDistance, shadowFadeStartDistance, mainCameraToFragmentMagnitude);
		float fragmentShadowedRatio = total / 8.0f;
		
		// return mix(1.0f, fragmentShadowedRatio, shadowStrength);
		return fragmentShadowedRatio;
	}
	else
	{
		return 1.0f;
	}
//#else
	return 0.0f;
//#endif
};

// --------------------------------------------------------------------------------
float calculateInShadowRatio(const vec3 surfaceWorldPosition)
{	
	// Needs to be revised
	// Shadow Map Cascades sampling
	const vec3 mainCameraToFragment = surfaceWorldPosition - ubo.worldCameraPosition.xyz;
	const uint cascadeSplitId = pickCascadeSplit(length(mainCameraToFragment), ubo.cascadeSplitsEndDistances);
	
	float inShadowRatio = 1.0f;
	if(cascadeSplitId < MAXIMUM_CASCADES)
	{
		// Calculate shadow map tex coord
		const vec4 homogeneousShadowMapSpaceFragment = ubo.worldToShadowMapMatrices[cascadeSplitId] * vec4(surfaceWorldPosition, 1.0f);
		const vec3 cartesianShadowMapFragment = homogeneousShadowMapSpaceFragment.xyz / homogeneousShadowMapSpaceFragment.w;
		const vec3 shadowMapSpaceFragment = vec3(cartesianShadowMapFragment.x, cartesianShadowMapFragment.y, 
											cartesianShadowMapFragment.z);
		
		// Sample the Shadow Map
		inShadowRatio = computeInShadowRatio(ubo.offsetScale, shadowMapSpaceFragment, ubo.worldCameraPosition.xyz, surfaceWorldPosition, 
																				ubo.shadowDrawDistance, ubo.shadowFadeStartDistance, cascadeSplitId);
	}
	
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
	const vec3 indirectDiffuseTerm = calculateIndirectDiffuseTerm(surfaceProperties.m_diffuseColour);
	const vec3 directDiffuseTerm = calculateDirectDiffuseTerm(surfaceProperties.m_diffuseColour, surfaceProperties.m_worldNormal);
	const vec3 specularTerm = calculateSpecularTerm(surfaceProperties.m_specularColour, surfaceProperties.m_worldPosition, 
		surfaceProperties.m_worldNormal, surfaceProperties.m_smoothness);
	const float inShadowRatio = calculateInShadowRatio(surfaceProperties.m_worldPosition);
	
	return indirectDiffuseTerm + (inShadowRatio * (directDiffuseTerm + specularTerm));
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