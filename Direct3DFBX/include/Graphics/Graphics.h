/* ---------------------------------------------------------------------------
** Team Bear King
** 2017 DigiPen Institute of Technology, All Rights Reserved.
**
** Color.h
**
** Author:
** - Hyung Jun Park	- park.hyungjun@digipen.edu
**
** Contributors:
** - <list in same format as author if applicable>
** -------------------------------------------------------------------------*/

#pragma once

#include <Windows.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include <d3dcommon.h>
#include <D3DX11async.h>
#include <PlatformWindow.h>
#include <GraphicsPipeline.h>

using namespace DirectX;

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------

//// Camera
//XMFLOAT3 InitTsl = XMFLOAT3(0.f, 0.f, 0.f);
//XMFLOAT3 InitRot = XMFLOAT3(0.f, 0.f, 0.f);
//static XMFLOAT3 tsl = InitTsl;
//static XMFLOAT3 rot = InitRot;
//static float scl = 1.0f;
//static float lrot = 0.f;
//
//UINT updateSpeed = 1;
//UINT frame = 0;
//UINT frame_per_sec = 0;

class Graphics
{
public:
	// Singleton
	static Graphics sInstance;

public:
	// Destructor
	~Graphics();

	// Initialise
	bool Initialize(const PlatformWindow& window, float near_dist, float far_dist);

	// Shutdown
	void Shutdown();

public:
	// Getters
	// Get Graphics pipeline
	GraphicsPipeline * GetPipeline();

private:
	// Constructor
	Graphics();

private:
	GraphicsPipeline mPipeline;

	friend class Shader;
};

#define GRAPHICS\
	Graphics::sInstance