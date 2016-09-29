//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType TextureVertexShader(VertexInputType input)
{
	PixelInputType output;

	output.Pos = input.Pos;

	//// Change the position vector to be 4 units for proper matrix calculations.
	//input.pos.w = 1.0f;

	//// Calculate the position of the vertex against the world, view, and projection matrices.
	//output.pos = mul(input.pos, worldMatrix);
	//output.pos = mul(output.pos, viewMatrix);
	//output.pos = mul(output.pos, projectionMatrix);
	
	// Store the texture coordinates for the pixel shader.
	output.Tex = input.Tex;

	return output;
}