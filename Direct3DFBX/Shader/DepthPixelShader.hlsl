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
float4 DepthPixelShader(PixelInputType input) : SV_TARGET
{
	return float4(input.shadowPos.z, input.shadowPos.z, input.shadowPos.z, 1.0f);
}
