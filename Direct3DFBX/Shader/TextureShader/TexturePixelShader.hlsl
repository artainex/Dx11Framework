Texture2D worldPosTexture : register(t0);
Texture2D worldNorTexture : register(t1);
Texture2D diffuseTexture : register(t2);
Texture2D specshineTexture : register(t3);
SamplerState SampleType : register(s0);

cbuffer cbMatrices : register(b0)
{
	matrix	World;
	matrix	View;
	matrix	Projection;
	matrix	WVP;
};

cbuffer cLights : register(b1)
{
	float4	l_ambient;
	float4	l_diffuse;
	float4	l_specular;
	float4	l_emissive;
	float3	l_direction;
};

struct PixelInputType
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	float4 worldpos = worldPosTexture.Sample(SampleType, input.Tex);
	float4 worldnor = worldNorTexture.Sample(SampleType, input.Tex);
	float4 diff = diffuseTexture.Sample(SampleType, input.Tex);
	float4 spec = specshineTexture.Sample(SampleType, input.Tex);

	// phong shading 
	// invert light direction for calculation
	float3 fvl_direction = normalize(-l_direction.xyz);
	float3 fvNorm = mul(float4(worldnor.xyz, 0.f), View).xyz;
	fvNorm = mul(float4(fvNorm, 0.f), Projection).xyz;
	float fNdotL = max(0, dot(fvNorm, fvl_direction));
	
	//float3 fvReflect = normalize(2.0f * fNdotL * worldnor.xyz - fvl_direction);
	//float3 fvViewDir = normalize(float3(0.0f, 0.f, -100.f) - worldpos.xyz);
	//float  fRdotV = saturate(dot(fvReflect, fvViewDir));
	//
	////--------------------------------------------------------------------------------------
	//// With Material
	////--------------------------------------------------------------------------------------
	//float3 fv_ambi = diff * l_ambient;
	//float3 fv_diff = (diff * l_diffuse * fNdotL).xyz;
	////float3 fv_spec = l_specular * pow(fRdotV, spec.w);
	////float3 fv_emit = l_emissive;
	//
	//// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
	//float3 final_Color = saturate(fv_ambi + fv_diff);
	//
	//// to implement normal map-need TBN matrix
	//
	////final_Color = saturate(final_Color + fv_spec);

	return worldpos;
}