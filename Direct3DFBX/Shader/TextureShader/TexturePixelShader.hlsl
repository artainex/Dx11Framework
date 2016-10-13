Texture2D worldPosTexture : register(t0);
Texture2D worldNorTexture : register(t1);
Texture2D diffuseTexture : register(t2);
Texture2D specshineTexture : register(t3);
Texture2D depthTexture : register(t4);
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

struct Light
{
	// For now, try use phong model, use ursine LightClass if I understand HDR or more (this class doesn't have HDR)
	float4 ambientColor;	// 16
	float4 diffuseColor;	// 32
	float4 specularColor;	// 48
	float4 lightPosition;	// 64
	float3 lightDirection;	// 76
	uint type;				// 80
};

// local lights
cbuffer cLocLights : register(b3)
{
	Light localLights[480];
};

// diffuse and specular calculation
float4 LightCalculation(float3 dir, float4 l_diff, float4 l_spec, float4 wPos, float4 wNor, float4 m_diff, float4 m_spec)
{
	float l_attenuation = 0.0f;
	float q_attenuation = 0.0002f;
	float dist_to_light = length(dir);

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
	
	return saturate((fv_diff + fv_spec) / (1 + l_attenuation * dist_to_light + q_attenuation * pow(dist_to_light, 2)));
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
	float2 uv = input.Tex;

	float4 worldpos = worldPosTexture.Sample(SampleType, uv);
	float4 worldnor = worldNorTexture.Sample(SampleType, uv);
	float4 diff = diffuseTexture.Sample(SampleType, uv);
	float4 spec = specshineTexture.Sample(SampleType, uv);
	float4 depth = depthTexture.Sample(SampleType, uv);
	
	//--------------------------------------------------------------------------------------
	// Ambient Light
	//--------------------------------------------------------------------------------------
	float4 fv_ambi = diff * al_ambient * 0.01f;

	//--------------------------------------------------------------------------------------
	// Global Light
	//--------------------------------------------------------------------------------------
	float4 global_Color = LightCalculation(gl_direction, gl_diffuse, gl_specular, worldpos, worldnor, diff, spec);
	
	// to implement normal map-need TBN matrix
	float4 local_Color = float4(0, 0, 0, 1.F);
	for (uint i = 0; i < 480; ++i)
	{
		local_Color = + LightCalculation(
				localLights[i].lightDirection,
				localLights[i].diffuseColor,
				localLights[i].specularColor,
				worldpos,
				worldnor,
				diff, spec);
	}

	local_Color = saturate(local_Color);
	
	return saturate(fv_ambi + local_Color + global_Color);
}
