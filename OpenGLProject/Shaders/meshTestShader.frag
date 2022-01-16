#version 450 core

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
	vec4 lightSourceDirection; // Already normalized
	mat4 model;
	mat4 viewProjection;
	mat4 worldToShadowMap;
	
	// Rule 1: Both the size and alignment are the size of the type in basic machine units.
	
	uint shadowMapTexelCount;
	
	bool pcfToggle;
	bool normalMapToggle;
	bool ambientToggle;
	bool diffuseToggle;
	bool specularToggle;
}ubo;

layout(binding = 0) uniform sampler2D ambientTextureSampler;
layout(binding = 1) uniform sampler2D diffuseTextureSampler;
layout(binding = 2) uniform sampler2D specularTextureSampler;
layout(binding = 3) uniform sampler2D normalMapTextureSampler;
layout(binding = 4) uniform sampler2D maskTextureSampler;

layout(binding = 5) uniform sampler2DShadow shadowMap;

uniform Material material;

float computeInShadowRatio(bool pcfEnabled, uint smTexelCount, vec3 shadowMapFragment)
{	
	vec3 currentFragmentDepthTextureCoords = shadowMapFragment * 0.5f + 0.5f;
	currentFragmentDepthTextureCoords.z -= 0.0005;
	
	return texture(shadowMap, currentFragmentDepthTextureCoords).r;
};

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
	const vec4 homogeneousShadowMapSpaceFragment = ubo.worldToShadowMap * vec4(out_worldSpaceFragment, 1.0f);
	const vec3 cartesianShadowMapFragment = homogeneousShadowMapSpaceFragment.xyz / homogeneousShadowMapSpaceFragment.w;
	const vec3 shadowMapSpaceFragment = vec3(cartesianShadowMapFragment.x, cartesianShadowMapFragment.y, 
											cartesianShadowMapFragment.z);
	
	const float inShadowRatio = computeInShadowRatio(ubo.pcfToggle, ubo.shadowMapTexelCount, shadowMapSpaceFragment);
    FragColour = ambient + (inShadowRatio * (diffuse + specular));
} 