#version 450 core

in vec3 out_worldSpaceNormal;
in vec3 out_worldSpaceFragment;
in vec2 out_textureCoordinate;
in vec4 out_Tangent;
in vec3 vn;

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
uniform sampler2D normalMapTextureSampler;

uniform Material material;


void main()
{
	if(texture(ambientTextureSampler, out_textureCoordinate).a < 0.5f) discard;
	if(texture(diffuseTextureSampler, out_textureCoordinate).a < 0.5f) discard;
	if(texture(specularTextureSampler, out_textureCoordinate).a < 0.5f) discard;
	
	//Normal map computation
	vec4 tangentSpaceNormal = ((texture(normalMapTextureSampler, out_textureCoordinate) * 2.0f) -1.0f);
	
	vec3 bitangent = out_Tangent.w * cross(vn, vec3(out_Tangent.xyz)); 
	
	vec3 vNout = normalize( tangentSpaceNormal.x * out_Tangent.xyz + tangentSpaceNormal.y * bitangent + tangentSpaceNormal.z * vn);
	
	//Intensities
	const vec3 ambientIntensity = vec3(0.8f, 0.8f, 0.8f);
	const vec3 lightSourceIntensity = vec3(0.723f, 0.535f, 0.1293f);
	
	const vec3 worldSpaceLightVector = vec3(1060.0f, 2700.0f, -83.0f);
	
	//Normalize
	const vec3 normalizedWorldSpaceLightVector = normalize(worldSpaceLightVector);
	const vec3 normalizedWorldSpaceNormal = normalize(vNout);
	//const vec3 normalizedWorldSpaceNormal = normalize(out_worldSpaceNormal);
	const vec3 normalizedToCameraDirection = normalize(ubo.worldCameraPosition.xyz);
	const vec3 normalizedFragmentToLightDirection = normalize(worldSpaceLightVector - out_worldSpaceFragment);
	
	//Ambient
	const vec4 ambient = vec4(ambientIntensity, 1.0f) * vec4(material.Ka, 1.0f) * texture(ambientTextureSampler, out_textureCoordinate);
	
	//Diffuse
	const float cosTheta = max(dot(normalizedFragmentToLightDirection, normalizedWorldSpaceNormal), 0.0f);
	const vec4 diffuse = (vec4(0.5f, 0.5f, 0.5f, 1.0f) * vec4(material.Kd, 1.0f)) * texture(diffuseTextureSampler, out_textureCoordinate)* cosTheta;
	
	//Specular
	const vec3 fragmentToCameraDirection = normalize(ubo.worldCameraPosition.xyz - out_worldSpaceFragment);
	
	const vec3 reflectedVector = normalize(reflect(-normalizedFragmentToLightDirection, normalizedWorldSpaceNormal));
	
	const float highlight = pow(max(dot(fragmentToCameraDirection, reflectedVector), 0.0f), material.Ns);
	const vec4 specular = (vec4(0.5f, 0.5f, 0.5f, 1.0f) * vec4(material.Ks, 1.0f)) * texture(specularTextureSampler, out_textureCoordinate) * highlight;

    FragColour = ambient + diffuse + specular;
} 