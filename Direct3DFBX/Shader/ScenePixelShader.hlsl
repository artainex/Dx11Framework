Texture2D worldPosTexture : register(t0);
Texture2D worldNorTexture : register(t1);
Texture2D diffuseTexture : register(t2);
Texture2D specshineTexture : register(t3);
Texture2D depthTexture : register(t4);
SamplerState SampleType : register(s0);

// 여기서 라이팅 계산을 해주면 된다는 거지
// 우선 라이트 정보를 넘기는 라이트 버퍼가 있어야 하겠네
cbuffer cbMatrices : register(b0)
{
	matrix	World;
	matrix	View;
	matrix	Projection;
	matrix	WVP;
};

cbuffer cbRenderMode : register(b1)
{
	uint renderMode;
	float3 padding;
}

struct PixelInputType
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ScenePixelShader(PixelInputType input) : SV_TARGET
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	float2 uv = input.Tex;

	float4 worldpos = worldPosTexture.Sample(SampleType, uv);
	float4 worldnor = worldNorTexture.Sample(SampleType, uv);
	float4 diff = diffuseTexture.Sample(SampleType, uv);
	float4 spec = specshineTexture.Sample(SampleType, uv);
	float4 depth = depthTexture.Sample(SampleType, uv);

	float4 light = lightTexture.Sample(SampleType, uv);
	
	if (1 == renderMode)
		return worldpos;
	else if (2 == renderMode)
		return worldnor;
	else if (3 == renderMode)
		return diff;
	else if (4 == renderMode)
		return spec;
	else if (5 == renderMode)
		return depth;
	else
		return diff;
}
