Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

cbuffer cbMaterial : register( b0 )
{
	float m_shineness;
	float m_transparency;
	float2 m_padding;
	float3 m_ambient;
	float3 m_diffuse;
	float3 m_specular;
	float3 m_emissive;
};

cbuffer cLights : register( b1 )
{
	float4	l_ambient;
	float4	l_diffuse;
	float4	l_specular;
	float4	l_emissive;
	float3	l_direction;
};

struct PS_INPUT
{
    float4	Pos	: POSITION;
	float3	Nor	: NORMAL;
	float2	Tex	: TEXCOORD0;
	float4	ViewDir 	: TEXCOORD1;
};

bool IsZeroColor(float4 num)
{
	if (num.x == 0.0f && num.y == 0.0f && num.z == 0.0f && num.w == 0.0f)
		return true;
	return false;
}

float4 PS( PS_INPUT input) : SV_Target
{	
	// texture map
	float2 uv = float2(input.Tex.x, input.Tex.y);
	float4 texColor = txDiffuse.Sample(samLinear, uv);

	// phong shading 
	// invert light direction for calculation
	float3 fvl_direction = normalize(-l_direction.xyz);
	float3 fvNorm = normalize(input.Nor);
	float fNdotL = max(0, dot(fvNorm, fvl_direction));
	
	float3 fvReflect	= normalize(2.0f * fNdotL * input.Nor - fvl_direction);
	float3 fvViewDir	= normalize( input.ViewDir );
	float  fRdotV		= saturate( dot(fvReflect, fvViewDir) );

	float3 fv_ambi = float4(m_ambient, 1) * l_ambient;
	float3 fv_diff = float4(m_diffuse, 1) * l_diffuse * fNdotL;
	float3 fv_spec = float4(m_specular, 1) * l_specular * pow(fRdotV, m_shineness);
	float3 fv_emit = float4(m_emissive, 1) * l_emissive;

	// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
	float3 final_Color = saturate(fv_ambi + fv_diff);

	// 첫째로, 디폴트 텍스쳐가 안먹혀. 이건 uv좌표가 셋팅이 안돼서 그런 것도 있음.
	// 둘째로 트랜스 패런시가 안나와
	//if ( !IsZeroColor(texColor) )
	//	final_Color *= texColor;
	
	final_Color = saturate( final_Color + fv_spec );

	return float4(final_Color, m_transparency);
}