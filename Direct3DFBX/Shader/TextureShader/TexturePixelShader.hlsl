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

// ambient light
cbuffer cAmbiLight : register(b1)
{
	float4	al_ambient;
	float4	al_diffuse;
	float4	al_specular;
	float4	al_position;
	float3	al_direction;
	uint	al_type;
};

// global light - for shadow too
cbuffer cGlobalLight : register(b2)
{
	float4	gl_ambient;
	float4	gl_diffuse;
	float4	gl_specular;
	float4	gl_position;
	float3	gl_direction;
	uint	gl_type;
};

//// local light1
cbuffer cLocLight1 : register(b3)
{
	float4	ll_ambient;
	float4	ll_diffuse;
	float4	ll_specular;
	float4	ll_position;
	float3	ll_direction;
	uint	ll_type;
};

//// local light2
//cbuffer cLocLight1 : register(b4)
//{
//	float4	ll_ambient;
//	float4	ll_diffuse;
//	float4	ll_specular;
//	float4	ll_position;
//	float3	ll_direction;
//	uint	ll_type;
//};

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

	//--------------------------------------------------------------------------------------
	// Ambient Light
	//--------------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------------
	// Global Light
	//--------------------------------------------------------------------------------------
	// invert light direction for calculation
	float3 fvl_direction = normalize(-gl_direction.xyz);
	float3 fvNorm = worldnor.xyz;
	float fNdotL = max(0, dot(fvNorm, fvl_direction));
	
	float3 fvReflect = normalize(2.0f * fNdotL * worldnor.xyz - fvl_direction);
	float3 fvViewDir = normalize(float3(0.0f, 0.f, -100.f) - worldpos.xyz);
	float  fRdotV = saturate(dot(fvReflect, fvViewDir));
	
	//--------------------------------------------------------------------------------------
	// With Material
	//--------------------------------------------------------------------------------------
	float3 fv_ambi = (float4(diff.xyz, 1) * gl_ambient).xyz;
	float3 fv_diff = (float4(diff.xyz, 1) * gl_diffuse * fNdotL).xyz;
	float3 fv_spec = gl_specular * pow(fRdotV, spec.w);
	
	// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
	float3 global_Color = saturate(fv_ambi + fv_diff);
	
	// to implement normal map-need TBN matrix	
	global_Color = saturate(global_Color + fv_spec);
	
	return float4(fv_diff.xyz, diff.w);
}