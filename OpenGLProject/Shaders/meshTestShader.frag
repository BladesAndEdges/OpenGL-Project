#version 450 core

in vec3 out_worldSpaceNormal;
in vec3 out_worldSpaceFragment;
in vec2 out_textureCoordinate;

out vec4 FragColour;

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
	mat4 model;
	mat4 viewProjection;
}ubo;

uniform sampler2D ambientTextureSampler;
uniform sampler2D diffuseTextureSampler;
uniform sampler2D specularTextureSampler;

uniform Material material;

void main()
{
	const vec3 worldSpaceLightVector = vec3(1060.0f, 2700.0f, -83.0f);
	
	//Normalize
	const vec3 normalizedWorldSpaceLightVector = normalize(worldSpaceLightVector);
	const vec3 normalizedWorldSpaceNormal = normalize(out_worldSpaceNormal);
	const vec3 normalizedToCameraDirection = normalize(ubo.worldCameraPosition.xyz);
	const vec3 normalizedFragmentToLightDirection = normalize(worldSpaceLightVector - out_worldSpaceFragment);
	
	//Ambient
	const vec4 ambient = vec4(material.Ka, 1.0f) * texture(ambientTextureSampler, out_textureCoordinate);
	
	//Diffuse
	const float cosTheta = max(dot(normalizedFragmentToLightDirection, normalizedWorldSpaceNormal), 0.0f);
	const vec4 diffuse = vec4(material.Kd, 1.0f) * texture(diffuseTextureSampler, out_textureCoordinate)* cosTheta;
	
	//Specular
	const vec3 fragmentToCameraDirection = normalize(ubo.worldCameraPosition.xyz - out_worldSpaceFragment);
	
	const vec3 reflectedVector = normalize(reflect(-normalizedFragmentToLightDirection, normalizedWorldSpaceNormal));
	
	const float highlight = pow(max(dot(fragmentToCameraDirection, reflectedVector), 0.0f), material.Ns);
	const vec4 specular = vec4(0.5f, 0.5f, 0.5f, 1.0f) * vec4(material.Ks, 1.0f) * texture(specularTextureSampler, out_textureCoordinate) * highlight;
	
    FragColour = diffuse + specular;
} 