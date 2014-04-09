
// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL0;
	float4 color		: COLOR;
	float2 uv			: TEXCOORD0;
	float3 toLight		: NORMAL1;
	// TODO Make a constant buffer for the pixel shader for light stuff.
	float4 lightAmbient : COLOR1;
	float4 lightDiffuse : COLOR2;
};

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	// Interpolation may have resulted in a non-unit directions, so re-normalize.
	input.normal = normalize(input.normal);
	input.toLight = normalize(input.toLight);

	// Ambient
	float4 ambient = input.color * input.lightAmbient;

	// Diffuse
	float diffuseIntensity = max(0, dot(input.normal, input.toLight));
	float4 diffuse = input.color * input.lightDiffuse * float4(diffuseIntensity, diffuseIntensity, diffuseIntensity, 1);

	return ambient + diffuse;
}