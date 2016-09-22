struct PerInstanceData
{
	matrix instanceMat;
};

StructuredBuffer<PerInstanceData>	g_pInstanceData :register(t0);

cbuffer cbMatrices : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	matrix WVP;
};

cbuffer cbMtxPalette : register(b1)
{
	matrix matPal[96];
};

struct VS_INPUT
{
	float4	Pos : POSITION;
	float3	Nor : NORMAL;
	float2	Tex : TEXCOORD;
};

struct VS_OUTPUT
{
	float4	Pos : SV_POSITION;
	float3	Nor : NORMAL;
	float2	Tex : TEXCOORD;
};

VS_OUTPUT vs_main(VS_INPUT input, uint instanceID : SV_InstanceID)
{
	VS_OUTPUT output;
	matrix instanceWVP = mul(Projection, View);
	instanceWVP = mul(instanceWVP, World);
	instanceWVP = mul(instanceWVP, g_pInstanceData[instanceID].instanceMat);

	instanceWVP = transpose(instanceWVP);

	output.Pos = mul(input.Pos, instanceWVP);
	output.Nor = mul(input.Nor, instanceWVP).xyz;
	output.Tex = input.Tex;

	return output;
}