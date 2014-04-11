
// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL0;
	float4 color		: COLOR;
	float2 uv			: TEXCOORD0;
	float3 toEye		: NORMAL1;
	float3 toLight		: NORMAL2;
	// TODO Make a constant buffer for the pixel shader for light stuff.
	float4 lightAmbient : COLOR1;
	float4 lightDiffuse : COLOR2;
};

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	// Interpolation may have resulted in a non-unit directions, so re-normalize.
	input.normal = normalize(input.normal);
	input.toEye = normalize(input.toEye);
	input.toLight = normalize(input.toLight);

	// Ambient
	float4 ambient = input.color * input.lightAmbient;

	// Diffuse and Specular
	float4 diffuse = float4(0, 0, 0, 0);
	float4 specular = float4(0, 0, 0, 0);
	float diffuseIntensity = dot(input.normal, input.toLight);
	if (diffuseIntensity > 0)
	{
		diffuse = input.color * input.lightDiffuse * float4(diffuseIntensity, diffuseIntensity, diffuseIntensity, 1);

		float3 lightReflect = reflect(-input.toLight, input.normal);
		float specularIntensity = dot(input.toEye, lightReflect);
		// TODO add shininess
		specularIntensity = pow(specularIntensity, 16);
		if (specularIntensity > 0)
		{
			// TODO use specular color values
			specular = float4(1, 1, 1, 1) * float4(.7, .7, .7, 1) * specularIntensity;
		}
	}

	return ambient + diffuse + specular;
}