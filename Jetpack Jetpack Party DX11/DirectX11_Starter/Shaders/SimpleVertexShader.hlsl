
// The constant buffer that holds our "per model" data
// - Each object you draw with this shader will probably have
//   slightly different data (at least for the world matrix)
cbuffer perModel : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

struct Light
{
	matrix world;
	float4 ambient;
	float4 diffuse;
	float4 specular;
};

cbuffer lights : register(b1)
{
	Light light;
}

// Defines what kind of data to expect as input
// - This should match our input layout!
struct VertexShaderInput
{
	float3 position		: POSITION;
	float3 normal		: NORMAL;
	float4 color		: COLOR;
	float2 uv			: TEXCOORD0;
};

// Defines the output data of our vertex shader
// - At a minimum, you'll need an SV_POSITION
// - Should match the pixel shader's input
struct VertexToPixel
{
	float4 position		: SV_POSITION;	// System Value Position - Has specific meaning to the pipeline!
	float3 normal		: NORMAL0;
	float4 color		: COLOR;
	float2 uv			: TEXCOORD0;
	float3 toEye		: NORMAL1;
	float3 toLight		: NORMAL2;
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
	
	// Transform normal assuming uniform scaling. Ignore translation.
	output.normal = mul(input.normal, (float3x3)world);
	output.normal = normalize(output.normal);

	// Pass the color through - will be interpolated per-pixel by the rasterizer
	output.color = input.color;

	// Calculate direction to eye.
	output.toEye = normalize(input.position - float3(view._41_42_43));

	// Calculate direction to light. Use fun math tricks to differentiate between point and directional lights.
	float4 lightPos = light.world._41_42_43_44;
	output.toLight = (lightPos.xyz * ((lightPos.a - 0.5) * 2))  - (input.position.xyz * lightPos.a);

	return output;
}