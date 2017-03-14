Texture2D txDiffuse : register( t0 ); 
Texture2D txNormal : register( t1 );
SamplerState basicSampler : register( s0 );

cbuffer cbMaterial : register( b0 )
{
	float4 m_ambient;
	float4 m_diffuse;
	float4 m_specular;
	float4 m_emissive;
	float m_transparency;
	float m_shineness;
	float2 m_padding;
	float4 m_colorandtype;
};

struct PS_INPUT
{
    float4	Pos			: SV_POSITION;
	float3	Nor			: NORMAL;
	float2	Tex			: TEXCOORD0;
	float4	WPos		: TEXCOORD1;
	float4	WNor		: TEXCOORD2;
};

struct PS_OUTPUT
{
	float4 Postion			: SV_Target0;
	float4 Normal			: SV_Target1;
	float4 Diffuse			: SV_Target2;
	float4 SpecularAndShine	: SV_Target3;
};

PS_OUTPUT GeometryPixelShader(PS_INPUT input)
{
	PS_OUTPUT output;

	float4 diffMap = txDiffuse.Sample(basicSampler, input.Tex);
	float4 normMap = txNormal.Sample(basicSampler, input.Tex);

	output.Postion = input.WPos;
	output.Normal = input.WNor;
	if(m_colorandtype.w == 0.0f)
		output.Diffuse = float4(m_diffuse.xyz, m_transparency);
	else if((m_colorandtype.w == 1.0f)) // for debug light
		output.Diffuse = float4(m_colorandtype.xyz, -1.f);
	output.SpecularAndShine = float4(m_specular.xyz, m_shineness);

	return output;
}