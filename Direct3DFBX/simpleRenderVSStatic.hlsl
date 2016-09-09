cbuffer cbMatrices : register( b0 )
{
	matrix	World;
    matrix	View;
	matrix	Projection;
	matrix	WVP;
};

cbuffer cLights : register(b1)
{
	float4	ambient;
	float4	diffuse;
	float4	specular;
	float4	emissive;
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
	float2	Tex		: TEXCOORD;
};

VS_OUTPUT vs_main(VS_INPUT input, uint instanceID : SV_InstanceID)
{
	VS_OUTPUT output;
	output.Pos = mul(float4(input.Pos.xyz, 1.f), WVP);
	output.Tex = input.Tex;
	return output;
}