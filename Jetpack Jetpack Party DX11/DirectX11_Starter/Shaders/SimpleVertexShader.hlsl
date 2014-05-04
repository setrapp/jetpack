
// The constant buffer that holds our "per model" data
// - Each object you draw with this shader will probably have
//   slightly different data (at least for the world matrix)
cbuffer perModel : register(b0)
{
	matrix world;
	matrix inverseTranspose;
	matrix view;
	matrix projection;
};

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

struct VertexShaderInput
{
	float3 position		: POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
};

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL0;
	float2 uv			: TEXCOORD0;
	float3 toEye		: NORMAL1;
	float3 toLight		: NORMAL2;
	float2 depth		: TEXCOORD1;
};

// The entry point for our vertex shader
VertexToPixel main( VertexShaderInput input )
{
	// Set up output
	VertexToPixel output;

	// Calculate output position
	matrix worldViewProj = mul(mul(world, view), projection);
	output.position = mul(float4(input.position, 1.0f), worldViewProj);
	output.uv = input.uv;
	
	// Transform normal by inverse transpose to remove scaling without affecting rotation.
	output.normal = mul(input.normal, (float3x3)inverseTranspose);
	output.normal = normalize(output.normal);

	// Calculate direction to eye.
	output.toEye = normalize(input.position - float3(view._41_42_43));

	// Calculate direction to light. Use fun math tricks to differentiate between point and directional lights.
	float4 lightPos = light.world._41_42_43_44;
	output.toLight = (lightPos.xyz * ((lightPos.a - 0.5) * 2))  - (input.position.xyz * lightPos.a);

	// Store depth and the range between the near and far planes.
	output.depth = float2(output.position.z, projectionInfo.x);

	return output;
}