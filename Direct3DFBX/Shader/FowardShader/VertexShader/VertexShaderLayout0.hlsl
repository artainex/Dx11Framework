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
};

struct VS_OUTPUT
{
    float4	Pos		: SV_POSITION;
	float3	Nor		: NORMAL;
	float4	ViewDir : TEXCOORD0;
};

VS_OUTPUT vs_main(VS_INPUT input, uint instanceID : SV_InstanceID)
{
	VS_OUTPUT output;

	output.Pos = mul(float4(input.Pos.xyz, 1.f), WVP);
	output.Nor = mul(float4(input.Nor.xyz, 0.f), WVP).xyz;
	output.ViewDir = float4(0.0f, 15.f, -50.f, 0.0f) - mul(float4(input.Pos.xyz, 1.f), World);

	return output;
}