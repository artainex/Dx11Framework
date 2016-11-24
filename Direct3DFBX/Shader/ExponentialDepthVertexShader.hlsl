////////////////////////////////////////////////////////////////////////////////
// Filename: exponentialdepth.vs
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
PixelInputType ExponentialDepthVertexShader(VertexInputType input)
{
	PixelInputType output;

	// Change the position vector to be 4 units for proper matrix calculations.
	output.Pos = float4(input.Pos.xyz, 1.f);

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.Pos = mul(output.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	//output.Pos = mul(float4(input.Pos, 1.f), WVP);

	// Store the shadowPos to the rendertarget
	output.shadowPos = output.Pos;

	return output;
}