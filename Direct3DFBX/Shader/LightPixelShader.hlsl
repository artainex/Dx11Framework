////////////////////////////////////////////////////////////////////////////////
// Filename: light.ps
////////////////////////////////////////////////////////////////////////////////
Texture2D worldPosTexture	: register(t0);
Texture2D worldNorTexture	: register(t1);
Texture2D diffuseTexture	: register(t2);
Texture2D specshineTexture	: register(t3);
Texture2D depthTexture		: register(t4);

SamplerState SampleType : register(s0);

const float PI = 3.14159265359f;

//////////////
// Globals //
//////////////
cbuffer LightBuffer : register(b0)
{
	float4 color;
	float3 eyePos;
	float3 lightPos;
	float lightRange;
	uint type;
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
float3 BRDF(float3 l, float3 v, float3 h, float3 n, float3 diffuse, float3 specular, float roughness, float3 color)
{
	float distribution = ((roughness + 2.0f) / (2.0f * PI)) * pow(max(dot(h, n), 0.0f), roughness);
	float3 fresnel = specular + (float3(1.0f, 1.0f, 1.0f) - specular) * pow(1 - max(dot(l, h), 0.0f), 5.0f);

	//float geometry = 1.0f / pow( max( dot( l, h ), 0.01f ), 2.0f );
	float geometry = 1.0f;
	return (diffuse / PI + ((distribution * fresnel * geometry) * 0.25f)) * max(dot(n, l), 0.0f) * color;
}

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
	
	float3 l = normalize(lightPos - wPos.xyz);
	float3 v = normalize(eyePos - wPos.xyz);
	float3 r = -reflect(l, wNor);
	float3 h = normalize(l + v);
	
	float lightDistance = length(lightPos - wPos.xyz);

	//// Shadow
	//float shadow = (castShadow == 0 || wPos.w == 0.0f) ? 1.0f : 0.0f;//ExponentialShadow(wPos);
	//shadow = (lightRange < 0.0f || lightDistance <= lightRange) ? shadow : 0.0f;
	
	// Attenuation
	float lightIntensity = (lightRange <= 0.0f) ? 1.0f : pow(1.0f - saturate(lightDistance / lightRange), 2.0f);
	
	// phong lighting - currently, use this instead of BRDF
	// BRDF is not working for some reason
	float3 fvl_direction = normalize(-l);
	float3 fvNorm = wNor.xyz;
	float fNdotL = max(0, dot(fvNorm, fvl_direction));

	float3 fvReflect = normalize(2.0f * fNdotL * wNor.xyz - fvl_direction);
	float3 fvViewDir = normalize(float3(0.0f, 0.f, -100.f) - wPos.xyz);
	float  fRdotV = saturate(dot(fvReflect, fvViewDir));

	float4 ambi = diffuse * color;
	float4 diff = diffuse * color * fNdotL;
	float4 spec = specular * color * pow(fRdotV, roughness);

	// if light type == ambient, just diff * color
	float3 lighting =
		(0 == type) ? ambi.xyz : lightIntensity * BRDF(l, v, h, wNor, diffuse.xyz, specular.xyz, roughness, color);
	
	return depth;// float4(diff.xyz + spec.xyz, 1.0f); // shadow * float4(lighting.xyz, 1.0f);
}
