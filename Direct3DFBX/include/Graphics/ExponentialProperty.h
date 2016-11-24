#pragma once

#include <Utilities.h>

using namespace DirectX;

const int MAX_KERNEL_RADIUS = 50;
const int BATCH_X_NUM = 64;
const int BATCH_Y_NUM = 64;

// Resolution Constant Buffer
struct ResolutionBufferType
{
	XMINT3 resolution;
	int32_t padding;
};

struct WeightBufferType
{
	XMFLOAT4 weights[2 * MAX_KERNEL_RADIUS + 1];
};
