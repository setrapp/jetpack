struct Light
{
	matrix world;
	float4 ambient;
	float4 diffuse;
};

cbuffer perFrame : register(b0)
{
	Light light;
}

// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float4 color		: COLOR;
	float2 uv			: TEXCOORD0;
};

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	// Temp light
	float3 lightDir = normalize(float3(0, -1, 3));

	// Interpolation may have resulted in a non-unit normal, so re-normalize.
	input.normal = normalize(input.normal);

	// Calculate dot product of normal and direction to light.
	float lightIntensity = dot(input.normal, -lightDir);

	return input.color * float4(lightIntensity, lightIntensity, lightIntensity, 1);
}