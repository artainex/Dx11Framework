////////////////////////////////////////////////////////////////////////////////
// Filename: AmbientPS.hlsl
////////////////////////////////////////////////////////////////////////////////
Texture2D worldPosTexture	: register(t0);
Texture2D worldNorTexture	: register(t1);
Texture2D diffuseTexture	: register(t2);
Texture2D specshineTexture	: register(t3);

SamplerState SampleType : register(s0);

//////////////
// Globals //
//////////////
cbuffer AmbientBuffer
{
	float4 ambient;
	float3 eye;
	float skyDimension;
	float screenWidth;
	float screenHeight;
	float ambientAmount;
	float padding;
};

//cbuffer RandomBuffer
//{
//	float4 hammersley[IBL_N];
//};

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4	Pos			: SV_POSITION;
	float2	Tex			: TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// Helper function
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 AmbientPS(PixelInputType input) : SV_Target
{
	float2 uv = float2(input.Pos.xy) / float2(screenWidth, screenHeight);

	// For now, just return ambient lighted diffuse
	return ambientAmount * ambient * diffuseTexture.Sample(SampleType, uv);
}