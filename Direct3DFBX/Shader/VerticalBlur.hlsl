////////////////////////////////////////////////////////////////////////////////
// Filename: VerticalBlurShader.cs
////////////////////////////////////////////////////////////////////////////////

Texture2D inTexture : register(t0);		// read only texture
SamplerState ClampSampleType
{
	Filter = MIN_MAG_MIP_LINEAR;

	AddressU = clamp;
	AddressV = clamp;
};

RWTexture2D< float4 > outTexture;		// write only texxture

cbuffer ResolutionBuffer
{
	int3 resolution;
	int padding;
};

cbuffer WeightBuffer
{
	float4 weights[2 * MAX_KERNEL_RADIUS + 1];
};

groupshared float4 cache[BATCH_Y_NUM + 2 * MAX_KERNEL_RADIUS];	// Variable shared with other threads in 128x1 thread group
																// Declare thread group size

////////////////////////////////////////////////////////////////////////////////
// Compute Shader
////////////////////////////////////////////////////////////////////////////////
[numthreads(1, BATCH_Y_NUM, 1)]
void VerticalBlurShader(int3 dispatchThreadID : SV_DispatchThreadID, int3 groupThreadID : SV_GroupThreadID)
{
	// Set left pixels
	if (groupThreadID.y < resolution.z)
	{
		float2 samplePos = float2(
			dispatchThreadID.x / (float)resolution.x, 
			(dispatchThreadID.y - resolution.z) / (float)resolution.y );
		cache[groupThreadID.y] = inTexture.SampleLevel(ClampSampleType, samplePos, 0);
	}

	//set right pixels
	if ((BATCH_Y_NUM - groupThreadID.y) <= resolution.z)
	{
		float2 samplePos = float2(
			dispatchThreadID.x / (float)resolution.x, 
			(dispatchThreadID.y + resolution.z) / (float)resolution.y );
		cache[groupThreadID.y + 2 * resolution.z] = inTexture.SampleLevel(ClampSampleType, samplePos, 0);
	}

	float2 samplePos = float2(
		dispatchThreadID.x / (float)resolution.x, 
		dispatchThreadID.y / (float)resolution.y);

	cache[resolution.z + groupThreadID.y] = inTexture.SampleLevel(ClampSampleType, samplePos, 0);

	// Wait for all threads to finish sampling
	GroupMemoryBarrierWithGroupSync();

	float4 blurColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//[unroll]	// Unroll the loop until it stops excuting
	for (int i = -resolution.z; i <= resolution.z; ++i)
	{
		blurColor += weights[i + resolution.z].x * cache[groupThreadID.y + i + resolution.z];
	}

	outTexture[dispatchThreadID.xy] = float4(blurColor.xyz, 1.0f);
}
