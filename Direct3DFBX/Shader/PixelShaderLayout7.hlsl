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
};

cbuffer cbMesh : register(b1)
{
	float4 mesh_color;
}

struct PS_INPUT
{
    float4	Pos			: SV_POSITION;
	float3	Nor			: NORMAL;
	float2	Tex			: TEXCOORD0;
	float4	WPos		: TEXCOORD1;
	float4	WNor		: TEXCOORD2;
	float4	Depth		: TEXCOORD3;
};

struct PS_OUTPUT
{
	float4 Postion: SV_Target0;
	float4 Normal: SV_Target1;
	float4 Diffuse: SV_Target2;
	float4 SpecularAndShine: SV_Target3;
	float4 Depth: SV_Target4;
};

PS_OUTPUT PS( PS_INPUT input) : SV_Target
{
	PS_OUTPUT output;

	float4 diffMap = txDiffuse.Sample(basicSampler, input.Tex);
	float4 normMap = txNormal.Sample(basicSampler, input.Tex);
	
	output.Postion = input.WPos;
	output.Normal = input.WNor;
	output.Diffuse = float4(mesh_color.xyz * m_diffuse.xyz, m_transparency) * diffMap;
	output.SpecularAndShine = float4(m_specular.xyz, m_shineness);

	float depth;
	depth = input.Depth.z / input.Depth.w;
	output.Depth = float4(depth, depth, depth, depth);

	return output;
}