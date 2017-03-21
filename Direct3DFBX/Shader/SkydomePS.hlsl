////////////////////////////////////////////////////////////////////////////////
// Filename: skydome.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer GradientBuffer
{
	float4 apexColor;
	float4 centerColor;
};

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 Pos : SV_POSITION;
	float4 domePos : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 SkydomePixelShader(PixelInputType input) : SV_TARGET
{
	float height;
	float4 outputColor;
	
	// Determine the position on the sky dome where this pixel is located.
	height = input.domePos.y;
	
	// The value ranges from -1.0f to +1.0f so change it to only positive values.
	if (height < 0.0)
		height = 0.0f;
	
	// Determine the gradient color by interpolating between the apex and center based on the height of the pixel in the sky dome.
	outputColor = lerp(centerColor, apexColor, height);
	
	return outputColor;
}
