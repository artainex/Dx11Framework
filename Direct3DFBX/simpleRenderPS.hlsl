Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

cbuffer cbMaterial : register( b0 )
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float4 emissive;
	float shineness;
	float transparency;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float4 Norm	: NORMAL;
};

float4 PS( PS_INPUT input) : SV_Target
{
	float4 lightVec = float4(0, -2, 1, 0);
	float2 newuv = float2(input.Tex.x, input.Tex.y);
	float4 tex_diff = txDiffuse.Sample(samLinear, newuv);
	float4 mat_diff = diffuse;
	float4 final = tex_diff + mat_diff;

	float lightVal = dot(lightVec, input.Norm);

	return float4(tex_diff.xyz, 1.f);
}