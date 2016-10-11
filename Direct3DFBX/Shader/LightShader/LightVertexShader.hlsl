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
	float4 Pos : POSITION0;
	float3 Nor : NORMAL;
	float2 Tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 Pos : SV_POSITION;
	float3 Nor : NORMAL;
	float2 Tex : TEXCOORD0;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
	PixelInputType output;

	// Change the position vector to be 4 units for proper matrix calculations.
	output.Pos = float4(input.Pos.xyz, 1.f);

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.Pos = mul(output.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	
	// Calculate the normal vector against the world matrix only.
	// Normalize the normal vector.
	output.Nor = normalize( mul(float4(input.Nor.xyz, 1.f), World).xyz );

	// Store the texture coordinates for the pixel shader.
	output.Tex = input.Tex;

	return output;
}