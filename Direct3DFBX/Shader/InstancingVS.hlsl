struct PerInstanceData
{
	matrix instanceMat;
};

StructuredBuffer<PerInstanceData>	g_pInstanceData :register( t0 );

cbuffer cbMatrices : register( b0 )
{
	matrix World;
    matrix View;
	matrix Projection;
	matrix WVP;
};

struct VS_INPUT
{
	float3 Pos : POSITION;
	float3 Nor : NORMAL;
	float2 Tex : TEXCOORD;
};

struct VS_OUTPUT
{
    float4	Pos		: SV_POSITION;
	float3	Nor		: NORMAL;
	float2	Tex		: TEXCOORD0;
	float4	WPos	: TEXCOORD1;
	float4	WNor	: TEXCOORD2;
	float4	Depth	: TEXCOORD3;
};

VS_OUTPUT vs_main(VS_INPUT input, uint instanceID : SV_InstanceID)
{
	VS_OUTPUT output;

	matrix instanceWVP = mul(Projection, View);
	instanceWVP = mul(instanceWVP, World);
	output.WPos = mul(float4(input.Pos, 1.f), instanceWVP);
	output.WNor = mul(float4(input.Nor, 0.f), instanceWVP);
	instanceWVP = mul(instanceWVP, g_pInstanceData[instanceID].instanceMat);
	instanceWVP = transpose(instanceWVP);

    output.Pos = mul(float4(input.Pos, 1.f), instanceWVP );
	output.Tex = input.Tex;
	output.Depth = output.Pos;

	return output;
}