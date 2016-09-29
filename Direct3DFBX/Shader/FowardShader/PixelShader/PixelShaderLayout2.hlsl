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
    float4	Pos			: POSITION;
	float3	Nor			: NORMAL;
	float4	ViewDir 	: TEXCOORD1;
};

float4 PS( PS_INPUT input) : SV_Target
{
	// phong shading 
	// invert light direction for calculation
	float3 fvl_direction = normalize(-l_direction.xyz);
	float3 fvNorm = normalize(input.Nor);
	float fNdotL = max(0, dot(fvNorm, fvl_direction));
	
	float3 fvReflect	= normalize(2.0f * fNdotL * input.Nor - fvl_direction);
	float3 fvViewDir	= normalize( input.ViewDir );
	float  fRdotV		= saturate( dot(fvReflect, fvViewDir) );

	//--------------------------------------------------------------------------------------
	// With Material
	//--------------------------------------------------------------------------------------
	float3 fv_ambi = float4(m_ambient, 1)	* l_ambient;
	float3 fv_diff = float4(m_diffuse, 1)	* l_diffuse * fNdotL;
	float3 fv_spec = float4(m_specular, 1)	* l_specular * pow(fRdotV, m_shineness);
	float3 fv_emit = float4(m_emissive, 1)	* l_emissive;

	//--------------------------------------------------------------------------------------
	// Without Material
	//--------------------------------------------------------------------------------------
	//float3 fv_ambi = l_ambient;
	//float3 fv_diff = l_diffuse * fNdotL;
	//float3 fv_spec = l_specular * pow(fRdotV, m_shineness);
	//float3 fv_emit = l_emissive;

	// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
	float3 final_Color = saturate(fv_ambi + fv_diff);

	// to implement normal map-need TBN matrix
	
	final_Color = saturate( final_Color + fv_spec );

	return float4(final_Color, m_transparency);
}