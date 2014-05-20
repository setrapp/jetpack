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
	float4 projectionInfo;
}

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL0;
	float2 uv			: TEXCOORD0;
	float3 toEye		: NORMAL1;
	float3 toLight		: NORMAL2;
	float2 depth		: TEXCOORD0;
};

Texture2D ambient : register(t0);
Texture2D diffuse : register(t1);
Texture2D normals : register(t2);
Texture2D extras : register(t3);
Texture2D toEyes : register(t4); //TODO remove when deferred lights work
SamplerState mySampler : register(s0);

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{	
	// Sample textures.
	float4 sampleAmbient = ambient.Sample(mySampler, input.uv);
	float4 sampleDiffuse = diffuse.Sample(mySampler, input.uv);
	float4 sampleNormal = normals.Sample(mySampler, input.uv);
	float4 sampleExtras = extras.Sample(mySampler, input.uv);

	// TODO remove when deferred lights work.
	float4 toEye = toEyes.Sample(mySampler, input.uv);
	float4 toLight = float4(sampleAmbient.w, sampleDiffuse.w, sampleNormal.w, 0);
	sampleAmbient.w = sampleDiffuse.w = sampleNormal.w = 1;

	// Ambient
	float4 finalAmbient = sampleAmbient * light.ambient;

	// Diffuse
	float4 finalDiffuse = float4(0, 0, 0, 0);
	float diffuseIntensity = dot(sampleNormal, input.toLight);
	if (diffuseIntensity > 0)
	{
		finalDiffuse = sampleDiffuse * light.diffuse * float4(diffuseIntensity, diffuseIntensity, diffuseIntensity, 1);
	}

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

	// TODO Depth totally does not work. Look into directly sampling the depth buffer.
	return finalAmbient + finalDiffuse;
}