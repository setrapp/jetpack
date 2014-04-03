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
	float4 color		: COLOR;
	float2 uv			: TEXCOORD0;
};

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	return input.color;
}