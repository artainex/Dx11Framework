Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

cbuffer cbMaterial : register( b0 )
{
	float4 m_ambient;
	float4 m_diffuse;
	float4 m_specular;
	float4 m_emissive;
	float m_shineness;
	float m_transparency;
};

cbuffer cLights : register(b1)
{
	float4	l_ambient;
	float4	l_diffuse;
	float4	l_specular;
	float4	l_emissive;
	float3	l_direction;
};

struct PS_INPUT
{
    float4 Pos	: POSITION;
	float4 Nor	: NORMAL;
	float2 Tex	: TEXCOORD;
};

float4 PS( PS_INPUT input) : SV_Target
{
	float4 lightVec = float4(l_direction.xyz, 0);
	float2 newuv = float2(input.Tex.x, input.Tex.y);
	float4 tex_diff = txDiffuse.Sample(samLinear, newuv);
	float4 mat_diff = m_diffuse;
	float4 light_diff = l_diffuse;
	float4 final = tex_diff + mat_diff * light_diff;

	float lightVal = dot(lightVec, input.Nor);

	return float4(final.xyz * lightVal, 1.f);
}