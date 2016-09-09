cbuffer cbGlobal : register( b0 )
{
	matrix	World;
    matrix	View;
	matrix	Projection;
	matrix	WVP;
	matrix	matPal[96];
};

struct VS_INPUT
{
    float3	Pos		: POSITION;
    float3	Nor		: NORMAL;
	float2	Tex		: TEXCOORD;
	float4	BWeight : BLENDWEIGHT;		//boneWeight2, boneWeight1, ...
	uint4	BIdx	: BLENDINDICES;		//boneID_1, boneID_2, boneID_3, boneID_4
};

struct VS_OUTPUT
{
    float4	Pos		: SV_POSITION;
	float2	Tex		: TEXCOORD;
	float4  Norm	: NORMAL;
};

VS_OUTPUT vs_main(VS_INPUT input, uint instanceID : SV_InstanceID)
{	
	VS_OUTPUT output;

	float4 weights = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = input.BWeight.x;
	weights[1] = input.BWeight.y;
	weights[2] = input.BWeight.z;
	weights[3] = input.BWeight.w;

	int indices[4] = { 0, 0, 0, 0 };
	indices[0] = input.BIdx.x;
	indices[1] = input.BIdx.y;
	indices[2] = input.BIdx.z;
	indices[3] = input.BIdx.w;

	matrix boneTransform;
	float3 pos = float3(0.f, 0.f, 0.f);
	float3 norm = float3(0, 0, 0);

	boneTransform = weights[0] * matPal[indices[0]];
	boneTransform += weights[1] * matPal[indices[1]];
	boneTransform += weights[2] * matPal[indices[2]];
	boneTransform += weights[3] * matPal[indices[3]];

	float3 vertPos = input.Pos.xyz;
	pos = mul(float4(vertPos, 1.0f), boneTransform).xyz;
	norm = mul(float4(input.Nor.xyz, 0.0f), boneTransform).xyz;

	output.Pos = mul(float4(pos.xyz, 1.f), WVP);
	output.Norm = mul(float4(norm, 0), World);
	output.Norm = mul(output.Norm, View);
	output.Tex = input.Tex;

	return output;
}