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
	float4	WPos	: TEXCOORD0;
	float4	WNor	: TEXCOORD1;
};

VS_OUTPUT vs_main(VS_INPUT input, uint instanceID : SV_InstanceID)
{
	VS_OUTPUT output;

	// ������� ���߿� �ٽ� ��Ȱ��Ű�� �ɷ� �ϰ�
	// ������ �׳� ���۵� �ٷ� �ع���
	output.Pos = mul(float4(input.Pos.xyz, 1.f), WVP);
	output.Nor = mul(float4(input.Nor.xyz, 0.f), WVP).xyz;
	output.WPos = mul(float4(input.Pos, 1.f), World);
	output.WNor = mul(float4(input.Nor, 0.f), World);

	return output;
}