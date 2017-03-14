////////////////////////////////////////////////////////////////////////////////
// Filename: light.ps
////////////////////////////////////////////////////////////////////////////////
Texture2D worldPosTexture	: register(t0);
Texture2D worldNorTexture	: register(t1);
Texture2D diffuseTexture	: register(t2);
Texture2D specshineTexture	: register(t3);
Texture2D depthTexture		: register(t4);

SamplerState SampleType : register(s0);

//////////////
// Globals //
//////////////
cbuffer LightBuffer : register(b0)
{
	float4 color;
	float3 eye;
	float screenWidth;
	float3 light;
	float screenHeight;
	float3 nearFarC;
	float lightRange;
}

cbuffer ShadowBuffer : register(b1)
{
	matrix shadowMatrix;
	int castShadow;
	int isExponential;
	int2 padding;
};

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
float ShadowCasting(float3 position, float nDotL, int isExponential)
{
	// transform world space into light projected space to compute the vertex position in the light's POV
	float4 shadowCrd = mul(float4(position.xyz, 1.f), shadowMatrix);

	// To make the coordinates in between -1~1, -1~1
	//float2 shadowIndex = shadowCrd.xy /= shadowCrd.w;
	float2 shadowIndex = (shadowCrd.w > 0.0f) ? shadowCrd.xy / shadowCrd.w : float2(-1.0f, -1.0f);
	
	//// 만약 이게 있어서 제대로 안나오는 거면 Texture를 이리로 보낼 때에 coordinate를 조작해준 게 분명해
	//// 이게 bias 매트릭스에 의한 것.
	//// To transform it to proper texture coord 0~1
	//shadowIndex.x = (shadowIndex.x + 1.0f) * 0.5f;
	//shadowIndex.y = (1.0f - shadowIndex.y) * 0.5f;

	float shadow = 1.0f;
	// If it's inside depth map
	if (shadowIndex.x >= 0.0f && shadowIndex.x <= 1.0f &&
		shadowIndex.y >= 0.0f && shadowIndex.y <= 1.0f)
	{
		if (0 == isExponential)
		{
			////////////////////////////////////////
			// General depth shadow
			////////////////////////////////////////
			float bias = 0.05f * tan(acos(nDotL));
			float depth = depthTexture.Sample(SampleType, shadowIndex).z;
			if (shadowCrd.z - bias > depth)
				return 0.0f;
		}
		else
		{
			////////////////////////////////////////
			// Exponential shadow
			////////////////////////////////////////
			float cDistance = clamp((shadowCrd.z - nearFarC.x) / (nearFarC.y - nearFarC.x), 0.0f, 1.0f);
			float expNegCZ = exp(-nearFarC.z * cDistance);
			float expCDepth = depthTexture.Sample(SampleType, shadowIndex).z;
			float expFactor = expCDepth * expNegCZ;

			return clamp(expFactor, 0.0f, 1.0f);
		}
	}
	return shadow;
}

////////////////////////////
// Physically Based Lighting
////////////////////////////
// The full BRDF with Lambertian diffuse term  and micro - facet specular term is :
// f(L, V, N) = Kd/π + D(H) * G(L, V, H) * F(L, H) / ( 4 * (L dot N)(V dot N) )
// where L, V are the unit length vectors toward the light and eye, 
// N is the surface normal, H is the half way vector L + V scaled to unit length.
// Phong with roughness parameter α : 0(rough)… ∞(mirror) 
//   D(H) = ((α + 2) / 2π) * (N dot H)^α
// A well known approximation to more carefully derived shadow terms : 
//   G(L, V, H) / (L dot N)(V dot N) => 1 / (L dot H)^2 or even … = 1
// Schlick's approximation of the Fresnel equation: 
//   F(L,H) = Ks + (1-Ks)(1-L dot H)^5
float3 BRDF(float3 l, float3 v, float3 h, float3 n, float3 diffuse, float3 specular, float roughness, float3 color)
{
	float distribution = ((roughness + 2.0f) / (2.0f * PI)) * pow(max(dot(h, n), 0.0f), roughness);
	float geometry = 1.0f / pow(max(dot(l, h), 0.01f), 2.0f);
	float3 fresnel = specular + (float3(1.0f, 1.0f, 1.0f) - specular) * pow(1 - max(dot(l, h), 0.0f), 5.0f);

	geometry = 1.f;
	return (diffuse / PI + (distribution * geometry * fresnel / 4.f)) * max(dot(l, n), 0.0f) * color;
	//return (diffuse / PI + geometry * 0.25f) * max(dot(n, l), 0.0f) * color;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPS(PixelInputType input) : SV_Target
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	float2 uv = float2(input.Pos.xy) / float2(screenWidth, screenHeight);

	float4 p = worldPosTexture.Sample( SampleType, uv );
	float3 n = worldNorTexture.Sample( SampleType, uv ).xyz;
	float3 l = normalize( light - p.xyz );
	float3 v = normalize( eye - p.xyz );
	float3 r = -reflect( l, n );
	float3 h = normalize( l + v );
	float4 diffuse = diffuseTexture.Sample( SampleType, uv );
	float4 specular = specshineTexture.Sample( SampleType, uv );
	float roughness = specular.w;
	specular.w = 1.f;

	// debug light model
	if (diffuse.w == -1.f)
		return float4(diffuse.xyz, 1.f);
		
	// BRDF
	float lightDistance = length( light - p.xyz );

	// Attenuation
	float lightIntensity = ( lightRange <= 0.0f ) ? 1.0f : pow(1.0f - saturate(lightDistance / lightRange), 2.0f);
	
	// if the depth from view is greater than depth from light view, then there's a shadow
	float shadow = (castShadow == 0 || p.w == 0.0f) ? 1.f : ShadowCasting( p, max(0, dot(n, l)), isExponential );
	shadow = (lightRange < 0.0f || lightDistance <= lightRange) ? shadow : 0.0f;

	float3 lighting = BRDF(l, v, h, n, diffuse.xyz, specular.xyz, roughness, color.xyz);

	// if the light is local, strength the intensity to give strong color
	if (color.w == 0.0f)
		lightIntensity *= 5.f;

	return lightIntensity * shadow * float4(lighting.xyz, 1.f);
}