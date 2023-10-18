#version 450 core

#define PI 3.1415926538
#define MAXIMUM_CASCADES 4
#define UINT_MAX 0xFFFFFFFFu

in vec3 v2f_worldSpaceNormal;
in vec3 v2f_worldSpaceFragment;
in vec2 v2f_textureCoordinate;
in vec4 v2f_Tangent;
in vec3 v2f_objectSpaceNormal;

out vec3 FragColour;

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

layout(std140, binding = 6) uniform sceneMatrices
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

layout(binding = 0) uniform sampler2D ambientTextureSampler;
layout(binding = 1) uniform sampler2D baseColourTextureSampler;
layout(binding = 2) uniform sampler2D metalnessTextureSampler;
layout(binding = 3) uniform sampler2D normalMapTextureSampler;
layout(binding = 4) uniform sampler2D maskTextureSampler;
layout(binding = 5) uniform sampler2DArrayShadow shadowMap;

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
}

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
}

// --------------------------------------------------------------------------------
vec3 getDebugColourByCascadeRegion(uint cascadeId)
{
	// Magenta
	vec3 debugColour = vec3(1.0f, 0.0f, 1.0f);
	
	if(cascadeId == 0u)
	{	
		debugColour = vec3(1.0f, 0.0f, 0.0f); // Red
	}
	else if(cascadeId == 1u)
	{
		debugColour = vec3(0.0f, 1.0f, 0.0f); // Green
	}
	else if(cascadeId == 2u)
	{
		debugColour = vec3(0.0f, 0.0f, 1.0f); // Blue
	}
	else if(cascadeId == 3u)
	{
		debugColour = vec3(1.0f, 1.0f, 0.0f); // Yellow
	}
	
	return debugColour;
}

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
		
		// return mix(1.0f, fragmentShadowedRatio, shadowStrength); // If the shadow should be affected by the fade
		return fragmentShadowedRatio; // Otherwise return the actual inShadowRatio
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
vec3 getWorldSurfaceNormal(bool normalMappingEnabled)
{
	if(normalMappingEnabled)
	{
		const vec4 tangentSpaceNormal = ((texture(normalMapTextureSampler, v2f_textureCoordinate) * 2.0f) -1.0f);
		const vec3 bitangent = v2f_Tangent.w * cross(v2f_objectSpaceNormal, vec3(v2f_Tangent.xyz)); 
		return normalize(tangentSpaceNormal.x * v2f_Tangent.xyz + tangentSpaceNormal.y * bitangent + tangentSpaceNormal.z * v2f_objectSpaceNormal);
	}
	else
	{
		return normalize(v2f_worldSpaceNormal);
	}
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
	return (metalness * baseColour) + (1.0f - metalness) * vec3(0.04f, 0.04f, 0.04f);
};

// --------------------------------------------------------------------------------
SurfaceProperties getSurfaceProperties()
{	
	SurfaceProperties surfaceProperties;
	
	// Position
	surfaceProperties.m_worldPosition = v2f_worldSpaceFragment;
	
	// Normal
	surfaceProperties.m_worldNormal = getWorldSurfaceNormal(ubo.normalMapToggle);
	
	// Material properties
	surfaceProperties.m_ambientColour = materialUniformData.m_ambientColour.xyz * texture(ambientTextureSampler, v2f_textureCoordinate).rgb;
	surfaceProperties.m_diffuseColour = getDiffuseColour();
	surfaceProperties.m_specularColour = getSpecularColour();
	surfaceProperties.m_smoothness = materialUniformData.m_specularHighLight;
	surfaceProperties.m_opacity = texture(maskTextureSampler, v2f_textureCoordinate).r;

	return surfaceProperties;
};

// --------------------------------------------------------------------------------
vec3 calculateIndirectDiffuseTerm(bool diffuseLightingEnabled, const vec3 surfaceDiffuseColour)
{
	if(diffuseLightingEnabled)
	{
		const vec3 c_globalLightSourceIntensity = vec3(0.8f, 0.8f, 0.8f);
		return c_globalLightSourceIntensity * surfaceDiffuseColour;
	}
	else
	{
		return vec3(0.0f, 0.0f, 0.0f);
	}
};

// --------------------------------------------------------------------------------
vec3 calculateDirectDiffuseTerm(bool diffuseLightingEnabled, const vec3 surfaceDiffuseColour, const vec3 worldNormal)
{
	if(diffuseLightingEnabled)
	{
		const vec3 c_lightSourceIntensity = vec3(0.723f, 0.535f, 0.1293f);
		const float c_cosTheta = max(dot(ubo.lightSourceDirection.xyz, worldNormal), 0.0f);
		return c_lightSourceIntensity * surfaceDiffuseColour * c_cosTheta;
	}
	else
	{	
		return vec3(0.0f, 0.0f, 0.0f);
	}
};

// --------------------------------------------------------------------------------
vec3 calculateSpecularTerm(bool specularLightingEnabled, const vec3 surfaceSpecularColour, const vec3 surfaceWorldNormal, const float surfaceSmoothness)
{
	if(specularLightingEnabled)
	{
		const vec3 c_lightSourceIntensity = vec3(0.723f, 0.535f, 0.1293f);
		
		const vec3 fragmentToCameraDirection = normalize(ubo.worldCameraPosition.xyz - v2f_worldSpaceFragment);
		const vec3 reflectedVector = normalize(reflect(-ubo.lightSourceDirection.xyz, surfaceWorldNormal));
		const float highlightDistribution = pow(max(dot(fragmentToCameraDirection, reflectedVector), 0.0f), surfaceSmoothness);
		return c_lightSourceIntensity * surfaceSpecularColour * highlightDistribution; // Ask if this is a correct term
	}
	else
	{
		return vec3(0.0f, 0.0f, 0.0f);
	}
};

// --------------------------------------------------------------------------------
vec3 calculateLightingAtSurfacePoint(SurfaceProperties surfaceProperties)
{	
	// Indirect Diffuse Term
	const vec3 indirectDiffuseTerm = calculateIndirectDiffuseTerm(ubo.diffuseToggle, surfaceProperties.m_diffuseColour);
	
	// Direct Diffuse Term
	const vec3 directDiffuseTerm = calculateDirectDiffuseTerm(ubo.diffuseToggle, surfaceProperties.m_diffuseColour, surfaceProperties.m_worldNormal);
	
	// Specular Term
	const vec3 specularTerm = calculateSpecularTerm(ubo.specularToggle, surfaceProperties.m_specularColour, surfaceProperties.m_worldNormal, surfaceProperties.m_smoothness);
	
	
	// Needs to be revised
	// Shadow Map Cascades sampling
	const vec3 mainCameraToFragment = v2f_worldSpaceFragment - ubo.worldCameraPosition.xyz;
	const uint cascadeSplitId = pickCascadeSplit(length(mainCameraToFragment), ubo.cascadeSplitsEndDistances);
	
	float inShadowRatio = 1.0f;
	if(cascadeSplitId < MAXIMUM_CASCADES)
	{
		// Calculate shadow map tex coord
		const vec4 homogeneousShadowMapSpaceFragment = ubo.worldToShadowMapMatrices[cascadeSplitId] * vec4(v2f_worldSpaceFragment, 1.0f);
		const vec3 cartesianShadowMapFragment = homogeneousShadowMapSpaceFragment.xyz / homogeneousShadowMapSpaceFragment.w;
		const vec3 shadowMapSpaceFragment = vec3(cartesianShadowMapFragment.x, cartesianShadowMapFragment.y, 
											cartesianShadowMapFragment.z);
		
		// Sample the Shadow Map
		inShadowRatio = computeInShadowRatio(ubo.offsetScale, shadowMapSpaceFragment, ubo.worldCameraPosition.xyz, v2f_worldSpaceFragment, 
																				ubo.shadowDrawDistance, ubo.shadowFadeStartDistance, cascadeSplitId);
	}
	
	if(ubo.cascadeDrawDistanceToggle)
	{
		return getDebugColourByCascadeRegion(cascadeSplitId) + (inShadowRatio * (directDiffuseTerm + specularTerm));
	}
	else
	{
		return indirectDiffuseTerm + (inShadowRatio * (directDiffuseTerm + specularTerm));
	}
};

// --------------------------------------------------------------------------------
void main()
{
	const float c_alphaTreshold = 0.5f;
	
	SurfaceProperties surfaceProperties = getSurfaceProperties();
	if(surfaceProperties.m_opacity < c_alphaTreshold) discard;
	
	FragColour =  calculateLightingAtSurfacePoint(surfaceProperties);
} 
