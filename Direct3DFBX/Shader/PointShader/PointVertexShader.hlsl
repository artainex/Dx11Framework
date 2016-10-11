cbuffer cbMatrices : register(b0)
{
	matrix	World;
	matrix	View;
	matrix	Projection;
	matrix	WVP;
};

struct VertexInputType
{
  float4 Pos : POSITION0;
  float4 Color : COLOR;
};

struct VertexOutputType
{
  float4 Pos : SV_POSITION;
  float4 Color : COLOR;
};

VertexOutputType PointVertexShader( VertexInputType input )
{
  // Change the position vector to be 4 units for proper matrix calculations.
  input.Pos.w = 1.f;

  return input;
}