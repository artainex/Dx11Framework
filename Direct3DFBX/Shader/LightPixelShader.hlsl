////////////////////////////////////////////////////////////////////////////////
// Filename: light.ps
////////////////////////////////////////////////////////////////////////////////
Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer
{
	float4 diffuseColor;
	float4 ambientColor;
	float4 specularColor;
	float4 emissiveColor;
	float3 lightDirection;
	uint type;
};

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 Pos : SV_POSITION;
	float3 Nor : NORMAL;
	float2 Tex : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInputType input) : SV_Target
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;
	
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.Tex);

	// Invert the light direction for calculations.
	lightDir = -lightDirection;
	
	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.Nor, lightDir));
	
	// Invert the light direction for calculations.
	lightDir = -lightDirection;
	
	// Calculate the amount of light on this pixel.
	lightIntensity = saturate( dot(input.Nor, lightDir) );
	
	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
	color = saturate(diffuseColor * lightIntensity);
	
	// Multiply the texture pixel and the final diffuse color to get the final pixel color result.
	//color = color * textureColor;
	
	return float4(1, 1, 1, 1);// color;
}
