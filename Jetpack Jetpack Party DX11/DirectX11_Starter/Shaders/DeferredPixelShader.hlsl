struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	uint4 shininess;
};

struct Light
{
	matrix world;
	float4 ambient;
	float4 diffuse;
	float4 specular;
};

cbuffer materialsAndLights : register(b1)
{
	Material material;
	Light light;
}

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL0;
	float2 uv			: TEXCOORD0;
	float3 toEye		: NORMAL1;
	float3 toLight		: NORMAL2;
};

Texture2D color : register(t0);
Texture2D diffuse : register(t0);
Texture2D normals : register(t0);
SamplerState mySampler : register(s0);
SamplerState textureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{	
	// Sample Texture.
	float4 sampleColor = normals.Sample(textureSampler, input.uv);

	// Extract color data.
	/*float4 inAmbient = material.ambient;
	float4 inDiffuse = material.diffuse;
	float4 inSpecular = material.specular;
	uint inShininess = material.shininess.r;
	
	// Interpolation may have resulted in a non-unit directions, so re-normalize.
	input.normal = normalize(input.normal);
	input.toEye = normalize(input.toEye);
	input.toLight = normalize(input.toLight);

	// Ambient
	float4 ambient = sampleColor * inAmbient * light.ambient;

	// Diffuse and Specular
	float4 diffuse = float4(0, 0, 0, 0);
	float4 specular = float4(0, 0, 0, 0);
	float diffuseIntensity = dot(input.normal, input.toLight);
	if (diffuseIntensity > 0)
	{
		diffuse = sampleColor * inDiffuse * light.diffuse * float4(diffuseIntensity, diffuseIntensity, diffuseIntensity, 1);

		float3 lightReflect = reflect(-input.toLight, input.normal);
		float specularIntensity = dot(input.toEye, lightReflect);
		specularIntensity = pow(specularIntensity, inShininess);
		if (specularIntensity > 0)
		{
			specular = sampleColor * inSpecular * light.specular * specularIntensity;
		}
	}*/


	return sampleColor;
}