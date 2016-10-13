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

struct PS_INPUT
{
    float4	Pos			: SV_POSITION;
	float3	Nor			: NORMAL;
	float4	WPos		: TEXCOORD0;
	float4	WNor		: TEXCOORD1;
	float4	Depth		: TEXCOORD2;
};

struct PS_OUTPUT
{
	float4 Postion: SV_Target0;
	float4 Normal: SV_Target1;
	float4 Diffuse: SV_Target2;
	float4 SpecularAndShine: SV_Target3;
	float4 Depth: SV_Target4;
};

PS_OUTPUT PS(PS_INPUT input)
{
	PS_OUTPUT output;
	
	output.Postion	= input.WPos;
	output.Normal	= input.WNor;
	output.Diffuse  = float4(m_diffuse.xyz, m_transparency);
	output.SpecularAndShine = float4(m_specular, m_shineness);

	float depth;
	float4 color;
	// First we get the depth value for this pixel, note that it is stored as z / w so we perform the necessary division to get the depth.
	// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
	depth = input.Depth.z / input.Depth.w;
	// The following section is where we determine how to color the pixel.
	// Remember the diagram at the top of the tutorial which describes how the first 10 % of the depth buffer contains 90 % of the float values.In this tutorial we will color that entire section as red.Then following that we will color a very tiny portion of the depth buffer green, notice it is only 0.025% in terms of precision but it takes up a large section close to the near plane.This green section helps you understand how quickly the precision falls off.The remaining section of the depth buffer is colored blue.
	// First 10% of the depth buffer color red.	
	output.Depth = depth;// float4(depth, depth, depth, depth);// color;

	return output;
}