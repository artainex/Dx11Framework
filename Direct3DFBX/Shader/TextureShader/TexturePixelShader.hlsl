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

// diffuse and specular calculation
float4 LightCalculation(float3 dir, float4 l_diff, float4 l_spec, float4 wPos, float4 wNor, float4 m_diff, float4 m_spec)
{
	// phong lighting
	float3 fvl_direction = normalize(-dir);
	float3 fvNorm = wNor.xyz;
	float fNdotL = max(0, dot(fvNorm, fvl_direction));

	float3 fvReflect = normalize(2.0f * fNdotL * wNor.xyz - fvl_direction);
	float3 fvViewDir = normalize(float3(0.0f, 0.f, -100.f) - wPos.xyz);
	float  fRdotV = saturate(dot(fvReflect, fvViewDir));

	//--------------------------------------------------------------------------------------
	// With Material
	//--------------------------------------------------------------------------------------
	float4 fv_diff = m_diff * l_diff * fNdotL;
	float4 fv_spec = m_spec * l_spec * pow(fRdotV, m_spec.w); // m_spec.w is shineness
	
	return saturate(fv_diff + fv_spec);
}

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
	
	//--------------------------------------------------------------------------------------
	// Ambient Light
	//--------------------------------------------------------------------------------------
	float4 fv_ambi = diff * al_ambient;

	//--------------------------------------------------------------------------------------
	// Global Light
	//--------------------------------------------------------------------------------------
	float4 global_Color = LightCalculation(gl_direction, gl_diffuse, gl_specular, worldpos, worldnor, diff, spec);
	
	//--------------------------------------------------------------------------------------
	// Local Lights
	//--------------------------------------------------------------------------------------
	float4 local_Color = LightCalculation(ll_direction, ll_diffuse, ll_specular, worldpos, worldnor, diff, spec);
	
	// to implement normal map-need TBN matrix
	
	return saturate(fv_ambi + local_Color + global_Color);
}
