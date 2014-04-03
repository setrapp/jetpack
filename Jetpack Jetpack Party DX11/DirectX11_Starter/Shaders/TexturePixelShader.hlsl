
// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float4 color		: COLOR;
	float2 uv			: TEXCOORD0;
};

Texture2D myTexture : register(t0);
SamplerState mySampler : register(s0);
// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{	
	return myTexture.Sample(mySampler, input.uv);
}