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
	int4 model_type;
}

struct PS_INPUT
{
    float4	Pos			: SV_POSITION;
	float3	Nor			: NORMAL;
	float4	WPos		: TEXCOORD0;
	float4	WNor		: TEXCOORD1;
};

struct PS_OUTPUT
{
	float4 Postion			: SV_Target0;
	float4 Normal			: SV_Target1;
	float4 Diffuse			: SV_Target2;
	float4 SpecularAndShine	: SV_Target3;
	float4 Type				: SV_Target4;
};

PS_OUTPUT GeometryPixelShader(PS_INPUT input)
{
	PS_OUTPUT output;
	
	output.Postion	= input.WPos;
	output.Normal	= input.WNor;
	output.Diffuse	= float4(mesh_color.xyz * m_diffuse.xyz, m_transparency);
	output.SpecularAndShine = float4(m_specular.xyz, m_shineness);
	output.Type = float4((float)model_type, 0, 0, 0);

	return output;
}