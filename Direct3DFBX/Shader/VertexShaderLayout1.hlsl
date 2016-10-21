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
	float2	Tex		: TEXCOORD;
};

struct VS_OUTPUT
{
    float4	Pos		: SV_POSITION;
	float3	Nor		: NORMAL;
	float2	Tex		: TEXCOORD0;
	float4	WPos	: TEXCOORD1;
	float4	WNor	: TEXCOORD2;
	//float4	Depth	: TEXCOORD3;
};

VS_OUTPUT vs_main(VS_INPUT input, uint instanceID : SV_InstanceID)
{
	VS_OUTPUT output;

	output.Pos = mul(float4(input.Pos, 1.f), WVP);
	output.Nor = mul(float4(input.Nor, 0.f), WVP).xyz;

	output.Tex = input.Tex;

	output.WPos = mul(float4(input.Pos, 1.f), World);
	output.WNor = mul(float4(input.Nor, 0.f), World);
	//output.Depth = mul(float4(input.Pos, 1.f), WVP);

	return output;
}