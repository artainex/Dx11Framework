cbuffer cbMatrices : register( b0 )
{
	matrix	World;
    matrix	View;
	matrix	Projection;
	matrix	WVP;
};

struct VS_INPUT
{
    float3	Pos		: POSITION;
    float3	Nor		: NORMAL;
	float3	Bin		: BINORMAL;
	float3	Tan		: TANGENT;
};

struct VS_OUTPUT
{
    float4	Pos		: SV_POSITION;
	float3	Nor		: NORMAL;
	float4	WPos	: TEXCOORD0;
	float4	WNor	: TEXCOORD1;
	//float4	Depth	: TEXCOORD2;
};

VS_OUTPUT GeometryVertexShader(VS_INPUT input)
{
	VS_OUTPUT output;

	output.Pos = mul(float4(input.Pos, 1.f), WVP);
	output.Nor = mul(float4(input.Nor, 0.f), WVP).xyz;
	output.WPos = mul(float4(input.Pos, 1.f), World);
	output.WNor = mul(float4(input.Nor, 0.f), World);
	//output.Depth = output.Pos;

	return output;
}