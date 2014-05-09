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
	float2 depth		: TEXCOORD1;
};

struct PixelOutput
{
	float4 ambient	: SV_TARGET0;
	float4 diffuse	: SV_TARGET1;
	float4 normal	: SV_TARGET2;
	float4 depth	: SV_TARGET3;
};

PixelOutput main(VertexToPixel input) : SV_TARGET
{
	// Extract color data.
	float4 inAmbient = material.ambient;
	float4 inDiffuse = material.diffuse;
	float4 inSpecular = material.specular;
	uint inShininess = material.shininess.r;
	
	// Interpolation may have resulted in a non-unit directions, so re-normalize.
	input.normal = normalize(input.normal);
	input.toEye = normalize(input.toEye);
	input.toLight = normalize(input.toLight);

	// Ambient
	/*float4 ambient = inAmbient * light.ambient;

	// Diffuse and Specular
	float4 diffuse = float4(0, 0, 0, 0);
	float4 specular = float4(0, 0, 0, 0);
	float diffuseIntensity = dot(input.normal, input.toLight);
	if (diffuseIntensity > 0)
	{
		diffuse = inDiffuse * light.diffuse * float4(diffuseIntensity, diffuseIntensity, diffuseIntensity, 1);

		float3 lightReflect = reflect(-input.toLight, input.normal);
		float specularIntensity = dot(input.toEye, lightReflect);
		specularIntensity = pow(specularIntensity, inShininess);
		if (specularIntensity > 0)
		{
			specular = inSpecular * light.specular * specularIntensity;
		}
	}*/

	// Depth
	float depth = input.depth.x / input.depth.y;

	PixelOutput output;
	output.ambient = inAmbient;
	output.diffuse = inDiffuse;
	output.normal = float4(input.normal, 0);
	output.depth = float4(depth, depth, depth, 1);
	return output;
}