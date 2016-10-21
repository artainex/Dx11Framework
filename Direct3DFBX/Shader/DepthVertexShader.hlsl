////////////////////////////////////////////////////////////////////////////////
// Filename: depth.vs
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
cbuffer cbMatrices : register(b0)
{
	matrix	World;
	matrix	View;
	matrix	Projection;
	matrix	WVP;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 Pos : SV_POSITION;
	float4 shadowPos : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType DepthVertexShader(VertexInputType input)
{
	PixelInputType Output;

	// Calculate the position
	Output.Pos = mul(float4(input.Pos.xyz, 1.0f), World);
	Output.Pos = mul(Output.Pos, View);
	Output.Pos = mul(Output.Pos, Projection);

	//Output.shadowPos = Output.Pos;

	return Output;
}