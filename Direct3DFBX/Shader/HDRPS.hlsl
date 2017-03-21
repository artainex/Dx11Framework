////////////////////////////////////////////////////////////////////////////////
// Filename: hdr.ps
////////////////////////////////////////////////////////////////////////////////
Texture2D worldPosTexture	: register(t0);
Texture2D worldNorTexture	: register(t1);
Texture2D diffuseTexture	: register(t2);
Texture2D specshineTexture	: register(t3);
Texture2D typeTexture		: register(t4);
Texture2D skyTexture		: register(t5);
Texture2D irradianceTexture	: register(t6);

SamplerState SampleType : register(s0);

static const float PI = 3.14159265359f;

//////////////
// Globals //
//////////////
cbuffer HDRBuffer
{
	float4 ambient;
	float3 eye;
	float skyDimension;
};

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4	Pos			: SV_POSITION;
	float2	Tex			: TEXCOORD0;
};

////////////////////////////
// Image Based Lighting
////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 HDRPS(PixelInputType input) : SV_Target
{
	float twoPI = 2.0f * PI;

	// Getting color datas
	float3 color = pow(diffuseTexture.Sample(SampleType, input.Tex).xyz, 2.2f);
	float4 specAndRough = specshineTexture.Sample(SampleType, input.Tex);
	float3 s = pow(specAndRough.xyz, 2.2f);
	
	// Getting other datas
	float3 p = worldPosTexture.Sample(SampleType, input.Tex).xyz;
	float3 normal = worldNorTexture.Sample(SampleType, input.Tex).xyz;
	float3 v = normalize(eye - p);
	float roughness = specAndRough.w;
	//float a = ambientTexture.Sample(SampleType, input.Tex).x;
	
	// Odds and ends Longitude-latitude Sphere map:
	// The map from a direction vector N to a texture coordinate (u,v) 
	//   uv = (1/2 - atan(N.y, N.x)/2PI, acos(N.z)/PI)
	// inverse map from a texture coordinate to a direction vector is: 
	//   uv = cos(2PI(1/2-u)) sin(¥ðv), sin(2PI(1/2-u)) sin(PIv), cos(PIv)
	float2 uv = float2(0.5f - (atan2(normal.z, normal.x) / (twoPI)), acos(normal.y) / PI);
	//float2 uv = float2( 0.5f + ( atan2( normal.z, normal.x ) / ( twoPI ) ), 0.5f - ( asin( normal.y ) / PI ) );
	
	// Diffuse term
	float3 diffuse = skyTexture.Sample(SampleType, uv).xyz; //pow( ambient.xyz, 2.2f ) * color.xyz / PI * irradianceTexture.SampleLevel( SampleType, uv, 0 ).xyz;

	return float4( pow(diffuse.xyz, 1.f / 2.2f), 1.f );

	//// Specular term
	//float3 R = 2.0f * dot(normal, v) * normal - v;
	//float3 A = normalize(cross(float3(0.0f, 0.0f, 1.0f), R));
	//float3 B = normalize(cross(R, A));
	//
	//float3 specular = float3(0.0f, 0.0f, 0.0f);
	//
	//int N = 1;
	//
	//for (int i = 0; i < N; ++i)
	//{
	//	float2 tex = float2(hammersley[i].x,
	//		acos(pow(hammersley[i].y, 1.0f / (roughness + 1.0f))) / PI);
	//
	//	float3 rl = float3(cos(twoPI * (0.5f - tex.x)) * sin(PI * tex.y),
	//		cos(PI * tex.y),
	//		sin(twoPI * (0.5f - tex.x)) * sin(PI * tex.y));
	//
	//	float3 l = normalize(rl.x * A + rl.z * B + rl.y * R);
	//
	//	float3 h = normalize(l + v);
	//
	//	float distribution = ((roughness + 2.0f) / (twoPI)) * pow(max(dot(h, normal), 0.0f), roughness);
	//	float3 fresnel = s + (float3(1.0f, 1.0f, 1.0f) - s) * pow(1 - max(dot(l, h), 0.0f), 5.0f);
	//
	//	float geometry = 1.0f;
	//
	//	float level = 0.5f * log2(skyDimension / N) - 0.5f * log2(distribution);
	//
	//	float2 luv = float2(0.5f - (atan2(l.z, l.x) / (twoPI)), acos(l.y) / PI);
	//
	//	specular += pow(ambient.xyz, 2.2f) * (geometry * fresnel * 0.25f) * max(dot(normal, l), 0.0f) * skyTexture.SampleLevel(SampleType, luv, level).xyz;
	//}
	//specular /= N;
	//
	//return float4(a * pow(diffuse.xyz + specular.xyz, 1.0f / 2.2f), 1.0f);
}