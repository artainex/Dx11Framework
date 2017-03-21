////////////////////////////////////////////////////////////////////////////////
// Filename: depth.ps
////////////////////////////////////////////////////////////////////////////////

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 Pos : SV_POSITION;
	float4 shadowPos : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 DepthPS(PixelInputType input) : SV_TARGET
{
	return input.shadowPos; // w coordinate cocntains depth
}