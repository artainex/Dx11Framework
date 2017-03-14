#pragma once

#include <ctime>
#include <Camera.h>
#include <Light.h>
#include <Graphics.h>

#include <RenderTarget.h>
#include <MultiRenderTarget.h>
#include <UAVTexture.h>

#include "Mesh.h"
#include "FBXModel.h"

#include <Core/GameComponent.h>
#include <Core/IDrawable.h>
#include <Shader.h>

#if DEBUG
#else
	#include "SpriteFont.h"
#endif

using namespace DirectX;
using namespace ursine::FBX_DATA;

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
// Resolution
const int Scr_Width = 1280;
const int Scr_Height = 768;
const float Near = 0.01f;
const float Far = 10000.f;

const float EPSILONF = 1.f - FLT_EPSILON;
 
// Light
const float lightNear = 0.1f;
const float lightFar = 1000.f;
const float exponentialConst = 45.f;

// Local Light Number
const int localLights = 30;

// Blur
const int MAX_KERNEL_RADIUS = 50;
const int BATCH_X_NUM = 128;
const int BATCH_Y_NUM = 72;

typedef std::unordered_map< std::string, ursine::FBXModel* > ModelList;	// Models
typedef std::unordered_map< std::string, Shader > ShaderList;			// Shader
typedef std::vector< Light >					GlobalLightList;
typedef std::vector< LocalLight >				LocalLightList;
typedef std::vector< DirectionalLight >			DirectionalLightList;

class Renderer : public GameComponent, public IDrawable
{
private:

#if DEBUG
#else
	//// font
	//DirectX::SpriteBatch*				g_pSpriteBatch = nullptr;
	//DirectX::SpriteFont*				g_pFont = nullptr;
#endif
	Graphics & mGraphics;

	// Matrices
	XMMATRIX						mPerspective;
	XMMATRIX						mOrthoMatrix;

	XMMATRIX						mScreenWorld;
	XMMATRIX						mScreenView;
	XMMATRIX						mScreenProj;

	XMMATRIX						mTranslMatrix;
	XMMATRIX						mScaleMatrix;
	XMMATRIX						mRotationMatrix;

	// Buffers
	ID3D11Buffer*					mMtxBuffer;
	ID3D11Buffer*					mMtxPaletteBuffer;

private:
	// Camera
	Camera							mCamera;

	// Lights
	Light							mAmbientLight;				// global ambient
	GlobalLightList					mGlobalLights;
	LocalLightList					mLocalLights;
	DirectionalLightList			mDirectionalLights;

	// Buffers
	RenderTarget					mSceneBuffer;				// scene buffer
	MultiRenderTarget				mGBuffer;					// geometry buffer
	RenderTarget					mDepthBuffer;				// depth buffer - FQS
	UAVTexture						mHBlurDepthBuffer;				// horizontal blur buffer
	UAVTexture						mFinalBlurDepthBuffer;			// final blur buffer (vertical after horizontal)
	RenderTarget					mHDRBuffer;					// hdr buffer - FQS

	// Blur
	XMFLOAT4						mWeights[2 * MAX_KERNEL_RADIUS + 1];	// Blur weights
	int								mBlurKernelRadius;						// Blur Kernel radius

	// Sky dome
	XMFLOAT4						mHammersley[IBL_N];
	ID3D11ShaderResourceView*		mSkydomeSRV;

	// HDR Maps
	HDRTexture						skyMap;
	HDRTexture						irradianceMap;

	XMFLOAT4						mSceneOffset;				// Scene offset
	XMFLOAT4						mSceneDimension;			// Scene dimension

private:
	ModelList						mModels;
	ShaderList						mShaders;

	Mesh							mQuad;						// Quad
	bool							mDrawDebugLight;
	bool							mExponential;

public:
	Renderer();
	~Renderer();

private:
	void GeometryPass();
	void AmbientPass();
	void LightPass();
	void DepthPass(Light& light);
	void ExponentialDepthPass(Light& light);
	void BuildWeights();
	void BlurDepth();
	void DrawFinalScene();
	
public:
	//--------------------------------------------------------------------------------------
	// GameComponent
	//--------------------------------------------------------------------------------------
	// Initialize
	void Initialize() override;

	// Shutdown
	void Shutdown() override;
	//--------------------------------------------------------------------------------------
	// Drawable
	//--------------------------------------------------------------------------------------
	// Draw
	void Draw(float dt) override;

	HRESULT InitShaders();
	HRESULT InitCamera();
	HRESULT InitLight();
	HRESULT InitModels();
	HRESULT InitHDR();
	void Update(double deltaTime);
	void Render();

	//// Draw Light (Debug)
	//template< typename LIGHTS >
	//void DrawLight(const LIGHTS & lights)
	//{
	//	// Texture for this instance
	//	mShaders["Flat"].SetTexture(PixelShader, 0,
	//		SYSTEM_MANAGER.GetGameComponentFromSystem<TextureManager>()->GetTexture("white"));
	//
	//	Mesh * light = SYSTEM_MANAGER.GetGameComponentFromSystem<MeshManager>()->GetMesh("Sphere");
	//
	//	for (auto & i : lights)
	//	{
	//		// Set Transform for this instance
	//		mShaders["Flat"].SetShaderParameters(VertexShader,
	//			0,
	//			(Matrix4::CreateTranslation(i.GetPosition()) *
	//				Matrix4::CreateScale(0.2f * Vector3f::ONE) *
	//				mGraphics.GetPipeline()->GetWorld()).Transpose(),
	//			mCamera->GetView().Transpose(),
	//			mGraphics.GetPipeline()->GetProjection().Transpose());
	//
	//		mShaders["Flat"].SetShaderParameters(PixelShader,
	//			0,
	//			i.GetColor());
	//
	//		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//		light->BindBuffers();
	//
	//		mShaders["Flat"].Render(light->GetIndexCount());
	//	}
	//}
};

#define BUFFER_INITIALIZE(expression, msg)\
	if(!expression)\
	{\
		CreateMessageBox(msg);\
		Shutdown();\
		return;\
	}

#include "Typing.h"
	DECLARE_TYPE(Renderer, Renderer);