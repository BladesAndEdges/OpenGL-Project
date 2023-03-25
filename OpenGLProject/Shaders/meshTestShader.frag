#version 450 core

#define PI 3.1415926538
#define MAXIMUM_CASCADES 4
#define UINT_MAX 0xFFFFFFFFu

in vec3 out_worldSpaceNormal;
in vec3 out_worldSpaceFragment;
in vec2 out_textureCoordinate;
in vec4 out_Tangent;
in vec3 vn;

out vec3 FragColour;

struct Material
{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float Ns;
};

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

layout(binding = 0) uniform sampler2D ambientTextureSampler;
layout(binding = 1) uniform sampler2D diffuseTextureSampler;
layout(binding = 2) uniform sampler2D specularTextureSampler;
layout(binding = 3) uniform sampler2D normalMapTextureSampler;
layout(binding = 4) uniform sampler2D maskTextureSampler;

layout(binding = 5) uniform sampler2DArrayShadow shadowMap;

uniform Material material;

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
vec3 getWorldSurfaceNormal(bool normalMappingEnabled)
{
	if(normalMappingEnabled)
	{
		const vec4 tangentSpaceNormal = ((texture(normalMapTextureSampler, out_textureCoordinate) * 2.0f) -1.0f);
		const vec3 bitangent = out_Tangent.w * cross(vn, vec3(out_Tangent.xyz)); 
		return normalize(tangentSpaceNormal.x * out_Tangent.xyz + tangentSpaceNormal.y * bitangent + tangentSpaceNormal.z * vn);
	}
	else
	{
		return normalize(out_worldSpaceNormal);
	}
};

// --------------------------------------------------------------------------------
SurfaceProperties getSurfaceProperties()
{	
	SurfaceProperties surfaceProperties;
	
	// Position
	surfaceProperties.m_worldPosition = out_worldSpaceFragment;
	
	// Normal
	surfaceProperties.m_worldNormal = getWorldSurfaceNormal(ubo.normalMapToggle);
	
	// Material properties
	surfaceProperties.m_ambientColour = material.Ka * texture(ambientTextureSampler, out_textureCoordinate).rgb;
	surfaceProperties.m_diffuseColour = material.Kd * texture(diffuseTextureSampler, out_textureCoordinate).rgb;
	surfaceProperties.m_specularColour = material.Ks * texture(specularTextureSampler, out_textureCoordinate).rgb;
	surfaceProperties.m_smoothness = material.Ns;
	surfaceProperties.m_opacity = texture(maskTextureSampler, out_textureCoordinate).r;

	return surfaceProperties;
};

// --------------------------------------------------------------------------------
vec3 calculateAmbientTerm(bool ambientLightingEnabled, const vec3 surfaceAmbientColour)
{
	if(ambientLightingEnabled)
	{
		const vec3 c_ambientIntensity = vec3(0.8f, 0.8f, 0.8f);
		return c_ambientIntensity * surfaceAmbientColour;
	}
	else
	{
		return vec3(0.0f, 0.0f, 0.0f);
	}
};

// --------------------------------------------------------------------------------
vec3 calculateDiffuseTerm(bool diffuseLightingEnabled, const vec3 surfaceDiffuseColour, const vec3 worldNormal)
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
		
		const vec3 fragmentToCameraDirection = normalize(ubo.worldCameraPosition.xyz - out_worldSpaceFragment);
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
	// Ambient Term
	const vec3 ambientTerm = calculateAmbientTerm(ubo.ambientToggle, surfaceProperties.m_ambientColour);
	
	// Diffuse Term
	const vec3 diffuseTerm = calculateDiffuseTerm(ubo.diffuseToggle, surfaceProperties.m_diffuseColour, surfaceProperties.m_worldNormal);
	
	// Specular Term
	const vec3 specularTerm = calculateSpecularTerm(ubo.specularToggle, surfaceProperties.m_specularColour, surfaceProperties.m_specularColour, surfaceProperties.m_smoothness);
	
	
	// Needs to be revised
	// Shadow Map Cascades sampling
	const vec3 mainCameraToFragment = out_worldSpaceFragment - ubo.worldCameraPosition.xyz;
	const uint cascadeSplitId = pickCascadeSplit(length(mainCameraToFragment), ubo.cascadeSplitsEndDistances);
	
	float inShadowRatio = 1.0f;
	if(cascadeSplitId < MAXIMUM_CASCADES)
	{
		// Calculate shadow map tex coord
		const vec4 homogeneousShadowMapSpaceFragment = ubo.worldToShadowMapMatrices[cascadeSplitId] * vec4(out_worldSpaceFragment, 1.0f);
		const vec3 cartesianShadowMapFragment = homogeneousShadowMapSpaceFragment.xyz / homogeneousShadowMapSpaceFragment.w;
		const vec3 shadowMapSpaceFragment = vec3(cartesianShadowMapFragment.x, cartesianShadowMapFragment.y, 
											cartesianShadowMapFragment.z);
		
		// Sample the Shadow Map
		inShadowRatio = computeInShadowRatio(ubo.offsetScale, shadowMapSpaceFragment, ubo.worldCameraPosition.xyz, out_worldSpaceFragment, 
																				ubo.shadowDrawDistance, ubo.shadowFadeStartDistance, cascadeSplitId);
	}
	
	if(ubo.cascadeDrawDistanceToggle)
	{
		return getDebugColourByCascadeRegion(cascadeSplitId) + ambientTerm + (inShadowRatio * (diffuseTerm + specularTerm));
	}
	else
	{
		return ambientTerm + (inShadowRatio * (diffuseTerm + specularTerm));
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
