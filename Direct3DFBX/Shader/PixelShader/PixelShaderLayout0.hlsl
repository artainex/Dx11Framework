cbuffer cbMaterial : register( b0 )
{
	float m_shineness;
	float m_transparency;
	float2 m_padding;
	float3 m_ambient;
	float3 m_diffuse;
	float3 m_specular;
	float3 m_emissive;
};

struct PS_INPUT
{
    float4	Pos			: SV_POSITION;
	float3	Nor			: NORMAL;
	float4	WPos		: TEXCOORD0;
	float4	WNor		: TEXCOORD1;
};

struct PS_OUTPUT
{
	float4 Postion: SV_Target0;
	float4 Normal: SV_Target1;
	float4 Diffuse: SV_Target2;
	float4 SpecularAndShine: SV_Target3;
};

PS_OUTPUT PS( PS_INPUT input)
{
	PS_OUTPUT output;
	
	output.Postion	= input.WPos;
	output.Normal	= input.WNor;
	output.Diffuse  = float4(m_diffuse.xyz, m_transparency);
	output.SpecularAndShine = float4(m_specular, m_shineness);

	return output;
}