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
void main()
{

	if(texture(maskTextureSampler, out_textureCoordinate).r < 0.5f) discard;
	
	vec3 shadingNormal = vec3(0.0f, 0.0f, 0.0f);
	
	//Normal map computation
	if(ubo.normalMapToggle)
	{
		vec4 tangentSpaceNormal = ((texture(normalMapTextureSampler, out_textureCoordinate) * 2.0f) -1.0f);
		vec3 bitangent = out_Tangent.w * cross(vn, vec3(out_Tangent.xyz)); 
		vec3 vNout = normalize( tangentSpaceNormal.x * out_Tangent.xyz + tangentSpaceNormal.y * bitangent + tangentSpaceNormal.z * vn);
		shadingNormal = vNout;
	}
	else
	{
		shadingNormal = normalize(out_worldSpaceNormal);
	}
	
	//Intensities
	const vec3 ambientIntensity = vec3(0.8f, 0.8f, 0.8f);
	const vec3 lightSourceIntensity = vec3(0.723f, 0.535f, 0.1293f);
	
	//Normalize
	const vec3 normalizedToCameraDirection = normalize(ubo.worldCameraPosition.xyz);
	
	//Ambient
	vec3 ambient = vec3(0.0f, 0.0f, 0.0f);
	if(ubo.ambientToggle)
	{
		ambient = ambientIntensity * material.Ka * texture(ambientTextureSampler, out_textureCoordinate).rgb;
	}
	
	//Diffuse
	vec3 diffuse = vec3(0.0f, 0.0f, 0.0f);
	if(ubo.diffuseToggle)
	{
		const float cosTheta = max(dot(ubo.lightSourceDirection.xyz, shadingNormal), 0.0f);
		diffuse = lightSourceIntensity * material.Kd * texture(diffuseTextureSampler, out_textureCoordinate).rgb * cosTheta;
	}
	
	//Specular
	vec3 specular = vec3(0.0f, 0.0f, 0.0f);
	if(ubo.specularToggle)
	{
		const vec3 fragmentToCameraDirection = normalize(ubo.worldCameraPosition.xyz - out_worldSpaceFragment);
		const vec3 reflectedVector = normalize(reflect(-ubo.lightSourceDirection.xyz, shadingNormal));
		const float highlight = pow(max(dot(fragmentToCameraDirection, reflectedVector), 0.0f), material.Ns);
		specular = lightSourceIntensity * material.Ks * texture(specularTextureSampler, out_textureCoordinate).rgb * highlight;
	}
	
	// SAMPLING THE SHADOW MAP
	
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
		FragColour = getDebugColourByCascadeRegion(cascadeSplitId) + ambient + (inShadowRatio * (diffuse + specular));
	}
	else
	{
		FragColour = ambient + (inShadowRatio * (diffuse + specular));
	}
} 
