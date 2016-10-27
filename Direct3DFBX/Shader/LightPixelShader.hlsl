////////////////////////////////////////////////////////////////////////////////
// Filename: light.ps
////////////////////////////////////////////////////////////////////////////////
Texture2D worldPosTexture	: register(t0);
Texture2D worldNorTexture	: register(t1);
Texture2D diffuseTexture	: register(t2);
Texture2D specshineTexture	: register(t3);
Texture2D depthTexture		: register(t4);

SamplerState SampleType : register(s0);

SamplerComparisonState ShadowSampler
{
	// sampler state
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = MIRROR;
	AddressV = MIRROR;

	// sampler comparison state
	ComparisonFunc = LESS;
};

static const float PI = 3.14159265359f;

//////////////
// Globals //
//////////////
cbuffer LightBuffer : register(b0)
{
	float4 color;
	float3 eyePos;
	float lightRange;
	float3 lightPos;
	uint type;
	matrix lightView;
	matrix lightProj;
};

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 Pos		: SV_POSITION;
	float2 Tex		: TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// Helper function
////////////////////////////////////////////////////////////////////////////////
float ShadowCasting(float3 position)
{
	// transform world space into light projected space
	float4 posShadowMap = mul( float4(position, 1.f), lightView );
	posShadowMap = mul( posShadowMap, lightProj );

	// Transform the position to shadow clip space
	float3 UVD = posShadowMap.xyz / posShadowMap.w;

	// Convert to shadow map UV values
	UVD.xy = 0.5 * UVD.xy + 0.5;
	UVD.y = 1.0 - UVD.y;

	return saturate(depthTexture.SampleCmpLevelZero(ShadowSampler, UVD.xy, UVD.z));
}

//float3 BRDF(float3 l, float3 v, float3 h, float3 n, float3 diffuse, float3 specular, float roughness, float3 color)
//{
//	float distribution = ((roughness + 2.0f) / (2.0f * PI)) * pow(max(dot(h, n), 0.0f), roughness);
//	float3 fresnel = specular + (float3(1.0f, 1.0f, 1.0f) - specular) * pow(1 - max(dot(l, h), 0.0f), 5.0f);
//
//	//float geometry = 1.0f / pow( max( dot( l, h ), 0.01f ), 2.0f );
//	float geometry = 1.0f;
//	return (diffuse / PI + ((distribution * fresnel * geometry) * 0.25f)) * max(dot(n, l), 0.0f) * color;
//}

//float ExponentialShadow(float3 p)
//{
//	float4 shadowCoord = mul(float4(p.xyz, 1.0f), shadowMatrix);
//	float shadow = 1.0f;
//	float2 shadowIndex = (shadowCoord.w > 0.0f) ? shadowCoord.xy / shadowCoord.w : float2(-1.0f, -1.0f);
//	// Inside depth map
//	if (shadowIndex.x >= 0.0f && shadowIndex.x <= 1.0f &&
//		shadowIndex.y >= 0.0f && shadowIndex.y <= 1.0f)
//	{
//		//float4 depth = depthTexture.Sample( sampleType, shadowIndex );
//		//if( shadowCoord.z > depth.z )
//		//	shadow = 0.0f;
//
//		float cDistance = clamp((shadowCoord.z - nearFarC.x) / (nearFarC.y - nearFarC.x), 0.0f, 1.0f);
//		float expNegCZ = exp(-nearFarC.z * cDistance);
//
//		float expCDepth = depthTexture.Sample(sampleType, shadowIndex).x;
//		float expFactor = expCDepth * expNegCZ;
//
//		shadow = clamp(expFactor, 0.0f, 1.0f);
//	}
//
//	return shadow;
//}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInputType input) : SV_Target
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	float2 uv = input.Tex;
	
	float4 wPos = worldPosTexture.Sample(SampleType, uv);
	float3 wNor = worldNorTexture.Sample(SampleType, uv).xyz;
	float4 diffuse = diffuseTexture.Sample(SampleType, uv);
	float4 specular = specshineTexture.Sample(SampleType, uv);
	float roughness = specular.w;
	specular.w = 1.f;
	float4 depth = depthTexture.Sample(SampleType, uv);
	
	float3 lightVector = wPos.xyz - lightPos;
	float3 lightDir = normalize(lightVector);
	float lightDistance = length(lightVector);

	// if the depth from view is greater than depth from light view, then there's a shadow
	float shadow = 0.f;
	if (1 <= type && type <= 3)
		shadow = ShadowCasting(wPos.xyz);

	// Attenuation
	float lightIntensity = (lightRange >= lightDistance) ? pow(1.0f - saturate(lightDistance / lightRange), 2.0f) : 0.0f;
	
	// phong lighting
	float3 fvNorm = wNor.xyz;
	float fNdotL = max(0, dot(fvNorm, -lightDir));

	float3 fvReflect = normalize(2.0f * fNdotL * wNor.xyz - lightDir);
	float3 fvViewDir = normalize(eyePos - wPos.xyz);
	float  fRdotV = saturate(dot(fvReflect, fvViewDir));

	// 일단 섀도 나오면 그 때 castShadow라는 불값 넣자
	// 타입이나 불 값 같은 콘스턴트 베리어블 어떻게 하면 더 효율적으로 셰이더에 전달할 수 있는지도 묻고
	float4 ambi = diffuse * color;
	float4 diff = diffuse * color * fNdotL;
	float4 spec = specular * color * pow(fRdotV, roughness);

	float4 finalColor;
	if (0 == type)
		finalColor = ambi;
	else 
		finalColor = shadow * lightIntensity * (diff + spec);

	return float4(finalColor.xyz, 1.0f);

	//// BRDF is not working for some reason
	//float3 l = normalize(lightPos - wPos.xyz);
	//float3 v = normalize(eyePos - wPos.xyz);
	//float3 r = -reflect(l, wNor);
	//float3 h = normalize(l + v);
	//float3 lighting = lightIntensity * BRDF(l, v, h, wNor, diffuse.xyz, specular.xyz, roughness, color);
	//return shadow * float4(lighting.xyz, 1.0f);
}
