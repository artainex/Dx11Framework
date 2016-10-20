////////////////////////////////////////////////////////////////////////////////
// Filename: light.ps
////////////////////////////////////////////////////////////////////////////////
Texture2D worldPosTexture : register(t0);
Texture2D worldNorTexture : register(t1);
Texture2D diffuseTexture : register(t2);
Texture2D specshineTexture : register(t3);
Texture2D depthTexture : register(t4);
SamplerState SampleType : register(s0);

//////////////
// Globals //
//////////////
cbuffer LightBuffer : register(b0)
{
	float4 color;
	float3 eyePos;
	float3 lightPos;
	float lightRange;
};

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 Pos		: SV_POSITION;
	float2 Tex		: TEXCOORD0;
	float4 WPos		: TEXCOORD1
};

////////////////////////////////////////////////////////////////////////////////
// Helper function
////////////////////////////////////////////////////////////////////////////////
float3 BRDF(float3 l, float3 v, float3 h, float3 n, float3 diffuse, float3 specular, float roughness, float3 color)
{
	float distribution = ((roughness + 2.0f) / (2.0f * PI)) * pow(max(dot(h, n), 0.0f), roughness);
	float3 fresnel = specular + (float3(1.0f, 1.0f, 1.0f) - specular) * pow(1 - max(dot(l, h), 0.0f), 5.0f);

	//float geometry = 1.0f / pow( max( dot( l, h ), 0.01f ), 2.0f );
	float geometry = 1.0f;
	return (diffuse / PI + ((distribution * fresnel * geometry) * 0.25f)) * max(dot(n, l), 0.0f) * color;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInputType input) : SV_Target
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	float2 uv = input.Tex;

	float4 worldpos = worldPosTexture.Sample(SampleType, uv);
	float4 worldnor = worldNorTexture.Sample(SampleType, uv);
	float4 diff = diffuseTexture.Sample(SampleType, uv);
	float4 spec = specshineTexture.Sample(SampleType, uv);
	float roughness = spec.w;
	float4 depth = depthTexture.Sample(SampleType, uv);

	float lightDistance = length(lightPos - worldpos.xyz);

	// Attenuation
	float lightIntensity = (lightRange <= 0.0f) ? 1.0f : pow(1.0f - saturate(lightDistance / lightRange), 2.0f);

	//float4 lighting = max( dot( n, l ), 0.0f ) * diffuse * color + pow( max( dot( h, n ), 0.0f ), roughness ) * specular;
	float3 lighting = BRDF(l, v, h, n, diffuse.xyz, specular.xyz, roughness, color);

	return lightIntensity * shadow * float4(lighting.xyz, 1.0f);
}
