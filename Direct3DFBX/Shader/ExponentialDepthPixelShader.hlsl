////////////////////////////////////////////////////////////////////////////////
// Filename: exponentialdepth.ps
////////////////////////////////////////////////////////////////////////////////
cbuffer ConstantBuffer : register(b0)
{
	float3 nearFarC;
	float padding;
};

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
float4 ExponentialDepthPixelShader(PixelInputType input) : SV_TARGET
{
	float d = input.shadowPos.w;
	// depth should be mapped between 0-1
	// (depth-minDepth) / (maxDepth - minDepth)
	float depth = clamp((d - nearFarC.x) / (nearFarC.y - nearFarC.x), 0.0f, 1.0f);
	
	// store e^cd instead of just depth d in the shadowmap
	float exponentialDepth = exp((float)nearFarC.z * depth);
	
	return float4(exponentialDepth, exponentialDepth, exponentialDepth, exponentialDepth);
}
