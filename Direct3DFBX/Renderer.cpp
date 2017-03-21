#include <MessageBox.h>
#include "Game.h"
#include "MeshLoader.h"
#include "Renderer.h"

#pragma warning (disable : 4239)

// Skydome
XMFLOAT4							apexColor = XMFLOAT4(0.0f, 0.15f, 0.66f, 1.0f);
XMFLOAT4							centerColor = XMFLOAT4(0.81f, 0.38f, 0.66f, 1.0f);

//======================================================================================
//
// Constuctor / Destructor
//
//======================================================================================
Renderer::Renderer()
	:
	mMtxBuffer(nullptr),
	mMtxPaletteBuffer(nullptr),
	mBlurKernelWidth(16),
	mGraphics(Graphics::sInstance),
	mDrawDebugLight(false),
	mExponential(true)
{
}

Renderer::~Renderer()
{
	Shutdown();
}

//======================================================================================
//
// Private Methods
//
//======================================================================================
//--------------------------------------------------------------------------------------
// Geometry Pass for deferred shading
//--------------------------------------------------------------------------------------
void Renderer::GeometryPass()
{
	// Clear the back buffer with Color rgba
	// and set G-buffer as render target
	mGBuffer.ClearRenderTargets();
	mGBuffer.SetRenderTargets();

	//RenderSkydome();
	//RenderLightModel();
	// 라이트 모델을 그리면 텍스쳐들이 제대로 바인딩되질 않아 왜지? 왜 텍스쳐가 제대로 안나와?
	// 셋 파라미터 잡아줄 때에 문제가 생긴모양인데 일단 이건 냅두고 로컬 라이트 적용부터
	// geo model
	if (!mModels["GeoModel"])
		return;

	// for all model
	auto& model = mModels["GeoModel"];

	// mesh nodes count
	size_t meshnodeCnt = model->GetMeshNodeCount();

	// xm matrix - row major
	// hlsl - column major
	// that's why we should transpose this	
	XMMATRIX world = XMMatrixTranspose(mGraphics.GetPipeline()->GetWorld());
	XMMATRIX view = XMMatrixTranspose(mCamera.GetView());
	XMMATRIX proj = XMMatrixTranspose(mPerspective);
	XMMATRIX wvp = XMMatrixTranspose(mGraphics.GetPipeline()->GetWorld() * mCamera.GetView() * mPerspective);

	// for all mesh nodes
	for (UINT mn_idx = 0; mn_idx < meshnodeCnt; ++mn_idx)
	{
		auto& material = model->GetNodeFbxMaterial(mn_idx);
		auto& mesh = model->GetMeshNode(mn_idx);

		// set shader parameters(mapping constant buffers, matrices, resources)
		mShaders["Geo"].SetShaderParameters(VertexShader, 0, world, view, proj, wvp);

		// materials
		mShaders["Geo"].SetShaderParameters(PixelShader, 0,
			material.mtrlConst.ambient,
			material.mtrlConst.diffuse,
			material.mtrlConst.specular,
			material.mtrlConst.emissive,
			material.mtrlConst.transparency,
			material.mtrlConst.shineness,
			XMFLOAT2(0, 0),
			XMFLOAT4(1, 1, 1, 0));

		mesh.Bind();

		mShaders["Geo"].Render(mesh.indexCount);
	}

	// 디버그 라이트 모델은 스피어 말고 그냥 구형 Flat모델 쓰고,
	// 일단 근처에 라이트 모아서 라이트 제대로 나오나 확인해보자
	//if (mDrawDebugLight)
	{
		// for all mesh nodes
		auto& light = mModels["Sphere"];
		auto& material = light->GetNodeFbxMaterial(0);
		auto& sphere = light->GetMeshNode(0);

		for (auto& iter : mLocalLights)
		{
			auto lpos = iter.GetPosition();
			float lscale = 0.5f;

			XMMATRIX lworld =
				XMMatrixScaling(lscale, lscale, lscale) *
				XMMatrixTranslation(lpos.X(), lpos.Y(), lpos.Z()) *
				mGraphics.GetPipeline()->GetWorld();

			XMMATRIX worldTrans = XMMatrixTranspose(lworld);
			XMMATRIX camTrans = XMMatrixTranspose(mCamera.GetView());
			XMMATRIX projTrans = XMMatrixTranspose(mGraphics.GetPipeline()->GetProjection());
			XMMATRIX wvpTrans = XMMatrixTranspose(lworld * mCamera.GetView() * mGraphics.GetPipeline()->GetProjection());
			XMFLOAT3 color = XMFLOAT3(iter.GetColor().X(), iter.GetColor().Y(), iter.GetColor().Z());

			// set shader parameters(mapping constant buffers, matrices, resources)
			mShaders["Geo"].SetShaderParameters(VertexShader, 0, worldTrans, camTrans, projTrans, wvpTrans);

			// materials
			mShaders["Geo"].SetShaderParameters(PixelShader, 0,
				material.mtrlConst.ambient,
				material.mtrlConst.diffuse,
				material.mtrlConst.specular,
				material.mtrlConst.emissive,
				XMFLOAT4(color.x, color.y, color.z, -1.f),
				material.mtrlConst.transparency,
				material.mtrlConst.shineness,
				XMFLOAT2(0, 0));

			sphere.Bind();

			mShaders["Geo"].Render(sphere.indexCount);
		}
	}
}

//--------------------------------------------------------------------------------------
// Ambient Pass for deferred shading
//--------------------------------------------------------------------------------------
void Renderer::AmbientPass()
{
	XMMATRIX world = XMMatrixTranspose(mScreenWorld);
	XMMATRIX view = XMMatrixTranspose(mScreenView);
	XMMATRIX proj = XMMatrixTranspose(mScreenProj);
	XMMATRIX wvp = XMMatrixTranspose(mScreenWorld * mScreenView * mScreenProj);

	// Ambient Light - should use FQS(full quad scene)
	mGraphics.GetPipeline()->SetZBuffer(false);

	// Adding textures
	mShaders["Ambient"].SetTexture(PixelShader, 0, &mGBuffer, 0);
	mShaders["Ambient"].SetTexture(PixelShader, 1, &mGBuffer, 1);
	mShaders["Ambient"].SetTexture(PixelShader, 2, &mGBuffer, 2);
	mShaders["Ambient"].SetTexture(PixelShader, 3, &mGBuffer, 3);

	// Set Vertex Shader Matrices
	auto &ambient = mAmbientLight.GetColor();
	auto &camPos = mCamera.GetPosition();

	mShaders["Ambient"].SetShaderParameters(VertexShader, 0, world, view, proj, wvp);
	mShaders["Ambient"].SetShaderParameters(PixelShader, 0,
		XMFLOAT4(ambient.X(), ambient.Y(), ambient.Z(), ambient.W()),
		XMFLOAT3(camPos.X(), camPos.Y(), camPos.Z()),
		0.0f, // will be replaced as skymap texture
		static_cast<float>(mSceneBuffer.GetWidth()),
		static_cast<float>(mSceneBuffer.GetHeight()),
		0.5f
	);
	//mShaders["Ambient"].SetShaderParameters(PixelShader, 1, mHammersley);

	mQuad.BindBuffers();

	mShaders["Ambient"].Render(mQuad.GetIndexCount());

	mShaders["Ambient"].UnBindTexture(PixelShader, 4);

	mGraphics.GetPipeline()->SetNoBlend();		// blending off
	mGraphics.GetPipeline()->SetZBuffer(true);	// depth test off
}

//--------------------------------------------------------------------------------------
// Light Pass for deferred shading
//--------------------------------------------------------------------------------------
void Renderer::LightPass()
{
	XMMATRIX world = XMMatrixTranspose(mScreenWorld);
	XMMATRIX view = XMMatrixTranspose(mScreenView);
	XMMATRIX proj = XMMatrixTranspose(mScreenProj);
	XMMATRIX wvp = XMMatrixTranspose(mScreenWorld * mScreenView * mScreenProj);

	// To move NPC coordinate to Screen space coordinate
	static const XMMATRIX bias = XMMatrixScaling(0.5f, -0.5f, 0.5f) * XMMatrixTranslation(0.5f, 0.5f, 0.5f);

	// Global light - should use FQS(full quad scene)
	{
		// create depth for global light
		for (auto &iter : mGlobalLights)
		{
			// if exponential, exponential depthpass,
			// else common depth
			(mExponential)? ExponentialDepthPass(iter) : DepthPass(iter);

			mSceneBuffer.SetRenderTarget();
			mGraphics.GetPipeline()->SetAdditiveBlend();
			mGraphics.GetPipeline()->SetZBuffer(false);
			mGraphics.GetPipeline()->SetBackFaceCull(true);

			// Adding textures
			mShaders["Lighting"].SetTexture(PixelShader, 0, &mGBuffer, 0);
			mShaders["Lighting"].SetTexture(PixelShader, 1, &mGBuffer, 1);
			mShaders["Lighting"].SetTexture(PixelShader, 2, &mGBuffer, 2);
			mShaders["Lighting"].SetTexture(PixelShader, 3, &mGBuffer, 3);
			mShaders["Lighting"].SetTexture(PixelShader, 4, &mDepthBuffer);
			// if exponential shadow
			if (mExponential)
				mShaders["Lighting"].BindTexture(PixelShader, 4, &mFinalBlurDepthBuffer);

			// Set Vertex Shader Matrices
			auto &global = iter.GetColor();
			auto &camPos = mCamera.GetPosition();
			auto &lpos = iter.GetPosition();

			mShaders["Lighting"].SetShaderParameters(VertexShader, 0, world, view, proj, wvp);
			mShaders["Lighting"].SetShaderParameters(PixelShader, 0,
				XMFLOAT4(global.X(), global.Y(), global.Z(), global.W()),
				XMFLOAT3(camPos.X(), camPos.Y(), camPos.Z()),
				static_cast<float>(mSceneBuffer.GetWidth()),
				XMFLOAT3(lpos.X(), lpos.Y(), lpos.Z()),
				static_cast<float>(mSceneBuffer.GetHeight()),
				XMFLOAT3(lightNear, lightFar, exponentialConst),
				-1.f	// when -1, there will be no attenuation
			);

			XMMATRIX shadowMatrix = iter.GetShadowView() * iter.GetShadowProjection() * bias;
			XMMATRIX shadowTrans = XMMatrixTranspose(shadowMatrix);
			mShaders["Lighting"].SetShaderParameters(PixelShader, 1, shadowTrans, 1, static_cast<int>(mExponential));

			mQuad.BindBuffers();

			mShaders["Lighting"].Render(mQuad.GetIndexCount());

			mShaders["Lighting"].UnBindTexture(PixelShader, 4);
		}
	}

	//// Directional Lights
	//{
	//	static const XMMATRIX dbias = XMMatrixTranslation(0.5f, 0.5f, 0.0f) * XMMatrixScaling(0.5f, -0.5f, 1.0f);
	//	for (auto & i : mDirectionalLights)
	//	{
	//		// Depth check
	//		XMMATRIX viewLight, perspLight;
	//		float lightNear, lightFar;
	//		//DirectionalDepthPass(i.GetPosition(), i.GetDirection(), i.GetDimension(), viewLight, perspLight, lightNear, lightFar);
	//	
	//		// Sampling traces
	//		XMMATRIX shadow = perspLight * viewLight;
	//		XMMATRIX invPV = mGraphics.GetPipeline()->GetProjection() * mCamera.GetView();
	//		XMVECTOR det;
	//		invPV = XMMatrixInverse(&det, invPV);
	//		//TraceSamplePass(i, shadow, invPV, lightNear, lightFar);
	//	}
	//}

	// Local Lights
	{
		auto& sphere = mModels["Sphere"]->GetMeshNode(0);
	
		mSceneBuffer.SetRenderTarget();
		mGraphics.GetPipeline()->SetAdditiveBlend();
		mGraphics.GetPipeline()->SetZBuffer(false);
		mGraphics.GetPipeline()->SetBackFaceCull(true);
	
		for (auto & iter : mLocalLights)
		{
			// Adding textures
			mShaders["Lighting"].SetTexture(PixelShader, 0, &mGBuffer, 0);
			mShaders["Lighting"].SetTexture(PixelShader, 1, &mGBuffer, 1);
			mShaders["Lighting"].SetTexture(PixelShader, 2, &mGBuffer, 2);
			mShaders["Lighting"].SetTexture(PixelShader, 3, &mGBuffer, 3);
	
			// Sphere
			auto &local = iter.GetColor();
			auto &camPos = mCamera.GetPosition();
			auto &lpos = iter.GetPosition();
			float lscale = iter.GetRadius();
	
			XMMATRIX lworld =
				XMMatrixScaling(lscale, lscale, lscale) *
				XMMatrixTranslation(lpos.X(), lpos.Y(), lpos.Z()) *
				mGraphics.GetPipeline()->GetWorld();
	
			XMMATRIX worldTrans = XMMatrixTranspose(lworld);
			XMMATRIX camTrans = XMMatrixTranspose(mCamera.GetView());
			XMMATRIX projTrans = XMMatrixTranspose(mGraphics.GetPipeline()->GetProjection());
			XMMATRIX wvpTrans = XMMatrixTranspose(lworld * mCamera.GetView() * mGraphics.GetPipeline()->GetProjection());
	
			mShaders["Lighting"].SetShaderParameters(VertexShader, 0, worldTrans, camTrans, projTrans, wvpTrans);
			mShaders["Lighting"].SetShaderParameters(PixelShader, 0,
				XMFLOAT4(local.X(), local.Y(), local.Z(), local.W()),
				XMFLOAT3(camPos.X(), camPos.Y(), camPos.Z()),
				static_cast<float>(mSceneBuffer.GetWidth()),
				XMFLOAT3(lpos.X(), lpos.Y(), lpos.Z()),
				static_cast<float>(mSceneBuffer.GetHeight()),
				XMFLOAT3(lightNear, lightFar, exponentialConst),
				iter.GetRadius());
	
			XMMATRIX shadowMatrix = XMMatrixIdentity();
	
			// for local light, if shadow is exponential or not is not important
			mShaders["Lighting"].SetShaderParameters(PixelShader, 1, shadowMatrix, 0, 0);
	
			sphere.BindCompatible();
	
			mShaders["Lighting"].Render(sphere.indexCount);
		}
	}

	// reset ZBuffer and No blending
	mGraphics.GetPipeline()->SetZBuffer(true);
	mGraphics.GetPipeline()->SetNoBlend();
}

//--------------------------------------------------------------------------------------
// Depth Pass for deferred shading
//--------------------------------------------------------------------------------------
void Renderer::DepthPass(Light& light)
{
	////bounding boxes
	//static ursine::SVec3 minPoint(-7.5f, -7.5f, -7.5f);
	//static ursine::SVec3 maxPoint(7.5f, 3.f, 7.5f);

	// minPoint + (maxPoint - minPoint) / 2.0f;
	ursine::SVec3 center = ursine::SVec3(0.f, 0.f, 0.f);

	//XMFLOAT3 boundingPoints[8] = { 
	//	minPoint,										//!< 000
	//	XMFLOAT3(minPoint.x, minPoint.y, maxPoint.z),	//!< 001 
	//	XMFLOAT3(minPoint.x, maxPoint.y, minPoint.z),	//!< 010 
	//	XMFLOAT3(minPoint.x, maxPoint.y, maxPoint.z),	//!< 011
	//	XMFLOAT3(maxPoint.x, minPoint.y, minPoint.z),	//!< 100
	//	XMFLOAT3(maxPoint.x, minPoint.y, maxPoint.z),	//!< 101
	//	XMFLOAT3(maxPoint.x, maxPoint.y, minPoint.z),	//!< 110
	//	XMFLOAT3(maxPoint.x, maxPoint.y, maxPoint.z),	//!< 111
	//};

	// Light looking at scene (Assuming global light is always outside the scene bounding box)
	ursine::SVec3 v = center - light.GetPosition();
	v.Normalize();

	if (fabs(v.Dot(ursine::SVec3::UnitY())) >= EPSILONF)
		light.GenerateShadowView(center, ursine::SVec3::UnitX());
	else
		light.GenerateShadowView(center, ursine::SVec3::UnitY());

	light.GenerateShadowProjection(XM_PI / 3.f, (float)Scr_Width / (float)Scr_Height, lightNear, lightFar);

	// Depth Checking
	mDepthBuffer.ClearRenderTarget();
	mDepthBuffer.SetRenderTarget();
	mGraphics.GetPipeline()->SetZBuffer(true);
	mGraphics.GetPipeline()->SetBackFaceCull(true);
	mGraphics.GetPipeline()->SetNoBlend();

	// geo model
	if (!mModels["GeoModel"])
		return;

	// for all model
	auto& geomodel = mModels["GeoModel"];

	// mesh nodes count
	size_t meshnodeCnt = geomodel->GetMeshNodeCount();

	// xm matrix - row major : S*R*T, W*V*P
	// hlsl - column major : 
	//
	// In column major: 	  In row major:
	// [x.x y.x z.x 0.0]	  [x.x x.y x.z 0.0]
	// [x.y y.y z.y 0.0]	  [y.x y.y y.z 0.0]
	// [x.z y.z z.z 0.0]	  [z.x z.y z.z 0.0]
	// [0.0 0.0 0.0 1.0]	  [0.0 0.0 0.0 1.0]
	//
	// that's why we should transpose this
	XMMATRIX world = XMMatrixTranspose(mGraphics.GetPipeline()->GetWorld());
	XMMATRIX view = XMMatrixTranspose(light.GetShadowView());
	XMMATRIX proj = XMMatrixTranspose(light.GetShadowProjection());
	XMMATRIX wvp = XMMatrixTranspose(mGraphics.GetPipeline()->GetWorld() * light.GetShadowView() * light.GetShadowProjection());

	// for all mesh nodes
	for (UINT mn_idx = 0; mn_idx < meshnodeCnt; ++mn_idx)
	{
		auto& mesh = geomodel->GetMeshNode(mn_idx);

		// Exponential Depth
		// set shader parameters(mapping constant buffers, matrices, resources)
		mShaders["Depth"].SetShaderParameters(VertexShader, 0, world, view, proj, wvp);

		mesh.Bind();

		mShaders["Depth"].Render(mesh.indexCount);
	}

	// reset back face cull
	mGraphics.GetPipeline()->SetBackFaceCull(false);

	// Reset rendertarget as scene buffer render target
	mSceneBuffer.SetRenderTarget();
}

void Renderer::ExponentialDepthPass(Light& light)
{
	////bounding boxes
	//static ursine::SVec3 minPoint(-7.5f, -7.5f, -7.5f);
	//static ursine::SVec3 maxPoint(7.5f, 3.f, 7.5f);
	//
	// minPoint + (maxPoint - minPoint) / 2.0f;
	ursine::SVec3 center = ursine::SVec3(0.f, 0.f, 0.f);

	// Light looking at scene (Assuming global light is always outside the scene bounding box)
	ursine::SVec3 v = center - light.GetPosition();
	v.Normalize();

	if (fabs(v.Dot(ursine::SVec3::UnitY())) >= EPSILONF)
		light.GenerateShadowView(center, ursine::SVec3::UnitX());
	else
		light.GenerateShadowView(center, ursine::SVec3::UnitY());

	light.GenerateShadowProjection(XM_PI / 3.f, (float)Scr_Width / (float)Scr_Height, lightNear, lightFar);

	// Depth Checking
	mDepthBuffer.ClearRenderTarget();
	mDepthBuffer.SetRenderTarget();
	mGraphics.GetPipeline()->SetZBuffer(true);
	mGraphics.GetPipeline()->SetBackFaceCull(true);
	mGraphics.GetPipeline()->SetNoBlend();

	// geo model
	if (!mModels["GeoModel"])
		return;

	// for all model
	auto& geomodel = mModels["GeoModel"];

	// mesh nodes count
	size_t meshnodeCnt = geomodel->GetMeshNodeCount();

	// xm matrix - row major
	// hlsl - column major
	// that's why we should transpose this
	XMMATRIX world = XMMatrixTranspose(mGraphics.GetPipeline()->GetWorld());
	XMMATRIX view = XMMatrixTranspose(light.GetShadowView());
	XMMATRIX proj = XMMatrixTranspose(light.GetShadowProjection());
	XMMATRIX wvp = XMMatrixTranspose(mGraphics.GetPipeline()->GetWorld() * light.GetShadowView() * light.GetShadowProjection());

	// for all mesh nodes
	for (UINT mn_idx = 0; mn_idx < meshnodeCnt; ++mn_idx)
	{
		auto& mesh = geomodel->GetMeshNode(mn_idx);

		// Exponential Depth
		// set shader parameters(mapping constant buffers, matrices, resources)
		mShaders["ExponentialDepth"].SetShaderParameters(VertexShader, 0, world, view, proj, wvp);
		mShaders["ExponentialDepth"].SetShaderParameters(PixelShader, 0, XMFLOAT3(lightNear, lightFar, exponentialConst));

		mesh.Bind();

		mShaders["ExponentialDepth"].Render(mesh.indexCount);
	}

	// reset back face cull
	mGraphics.GetPipeline()->SetBackFaceCull(false);

	// Reset rendertarget as scene buffer render target
	mSceneBuffer.SetRenderTarget();
	BlurDepth();
}

void Renderer::BuildGaussianBlurFilter()
{
	// Calculate Beta * e^(i^2 / 2*s^2) to create Gaussain Blur Filter

	// 2 * w + 1
	unsigned int kernalWidth = 2 * mBlurKernelWidth + 1;	
	float s = mBlurKernelWidth / 2; // s = w/2
	float sum = 0.f;
	for (unsigned int i = 0; i < kernalWidth; ++i)
	{
		float x = static_cast<float>(i) - static_cast<float>(mBlurKernelWidth);
		mWeights[i].x = std::exp(-((x * x) / (2.f * s * s))); // e^(i^2 / 2*s^2)
		sum += mWeights[i].x;
	}

	// Normalization = Beta
	for (unsigned int i = 0; i < kernalWidth; ++i)
		mWeights[i].x /= sum;
}

void Renderer::BlurDepth()
{
	// horizontal blur
	mShaders["HBlur"].SetShaderParameters(ComputeShader, 0, mDepthBuffer.GetWidth(), mDepthBuffer.GetHeight(), mBlurKernelWidth);
	mShaders["HBlur"].SetShaderArrayParameters(ComputeShader, 1, mWeights);
	mShaders["HBlur"].SetTexture(ComputeShader, 0, &mDepthBuffer);
	mShaders["HBlur"].BindUnorderedTexture(0, &mHBlurDepthBuffer);
	mShaders["HBlur"].Dispatch(mDepthBuffer.GetWidth() / BATCH_X_NUM, mDepthBuffer.GetHeight(), 1);
	mShaders["HBlur"].UnBindUnorderedTexture(0);

	// vertical blur
	mShaders["VBlur"].SetShaderParameters(ComputeShader, 0, mDepthBuffer.GetWidth(), mDepthBuffer.GetHeight(), mBlurKernelWidth);
	mShaders["VBlur"].SetShaderArrayParameters(ComputeShader, 1, mWeights);
	mShaders["VBlur"].BindTexture(ComputeShader, 0, &mHBlurDepthBuffer);
	mShaders["VBlur"].BindUnorderedTexture(0, &mFinalBlurDepthBuffer);
	mShaders["VBlur"].Dispatch(mHBlurDepthBuffer.GetWidth(), mHBlurDepthBuffer.GetHeight() / BATCH_Y_NUM, 1);
	mShaders["VBlur"].UnBindTexture(ComputeShader, 0);
	mShaders["VBlur"].UnBindUnorderedTexture(0);
}

//--------------------------------------------------------------------------------------
// Draw the final result of the scene
//--------------------------------------------------------------------------------------
void Renderer::DrawFinalScene()
{
	// Final Result Rendering
	mGraphics.GetPipeline()->SetZBuffer(false);

	XMMATRIX world = XMMatrixTranspose(mScreenWorld);
	XMMATRIX view = XMMatrixTranspose(mScreenView);
	XMMATRIX proj = XMMatrixTranspose(mScreenProj);
	XMMATRIX wvp = XMMatrixTranspose(mScreenWorld * mScreenView * mScreenProj);

	mShaders["SceneQuad"].SetShaderParameters(VertexShader, 0, world, view, proj, wvp);

	// Texturing
	mShaders["SceneQuad"].SetTexture(PixelShader, 0, &mSceneBuffer);

	mQuad.BindBuffers();

	mShaders["SceneQuad"].Render(mQuad.GetIndexCount());

	mGraphics.GetPipeline()->SetZBuffer(true);
}

//======================================================================================
//
// Public Methods
//
//======================================================================================
// Initialize
void Renderer::Initialize()
{
	// Initialize the world matrices
	mGraphics.Initialize(GAME.Window, Near, Far);
	mGraphics.GetPipeline()->SetWorld(XMMatrixIdentity());

	FAIL_CHECK_VOID_WITH_MSG(InitLight(), "Initialize Lights Failed.");
	FAIL_CHECK_VOID_WITH_MSG(InitCamera(), "Initialize Camera Failed.");

	DirectX::XMVECTOR eyev = DirectX::XMLoadFloat3(&XMFLOAT3(0.f, 0.f, -1.f));
	DirectX::XMVECTOR targetv = DirectX::XMLoadFloat3(&XMFLOAT3(0.f, 0.f, 0.f));
	DirectX::XMVECTOR upv = DirectX::XMLoadFloat3(&XMFLOAT3(0.f, 1.f, 0.f));

	mScreenWorld = XMMatrixTranslation(-0.5f, -0.5f, 0.f) * XMMatrixScaling(2.f, 2.f, 2.f);
	mScreenView = XMMatrixLookAtLH(eyev, targetv, upv);
	mScreenProj = XMMatrixOrthographicLH(1.f, 1.f, Near, Far);

	if (mExponential)
		BuildGaussianBlurFilter();

	// fbx models
	FAIL_CHECK_VOID_WITH_MSG(InitModels(), "Initialize FBX Models Failed.");

	// shaders
	FAIL_CHECK_VOID_WITH_MSG(InitShaders(), "Initialize Shaders Failed.");

	// buffers
	// create the geometry buffer multi render target which covers Full Screen Quad
	BUFFER_INITIALIZE(mGBuffer.Initialize(Scr_Width, Scr_Height),
		"Initialize Gbuffer render target Failed.");

	// create the scene render target which covers Full Screen Quad
	BUFFER_INITIALIZE(mSceneBuffer.Initialize(Scr_Width, Scr_Height),
		"Initialize the Scenebuffer render target Failed.");

	// create the blur buffer covers Full Screen Quad
	BUFFER_INITIALIZE(mDepthBuffer.Initialize(Scr_Width, Scr_Height),
		"Initialize the DepthBuffer render target Failed.");

	// create the blur buffer covers Full Screen Quad
	BUFFER_INITIALIZE(mHBlurDepthBuffer.Initialize(Scr_Width, Scr_Height),
		"Initialize the HBlurbuffer render target Failed.");

	// create the blur buffer covers Full Screen Quad
	BUFFER_INITIALIZE(mFinalBlurDepthBuffer.Initialize(Scr_Width, Scr_Height),
		"Initialize the FinalBlurbuffer render target Failed.");

	MeshLoader::CreateQuad(mQuad);

	mGraphics.GetPipeline()->SetBackFaceCull(true);
	mGraphics.GetPipeline()->SetZBuffer(true);
}

// Shutdown
void Renderer::Shutdown()
{
	SAFE_RELEASE(mSkydomeSRV);
	SAFE_RELEASE(mMtxBuffer);
	SAFE_RELEASE(mMtxPaletteBuffer);

	// shaders
	for (auto& iter : mShaders)
		iter.second.Shutdown();
	mShaders.clear();

	// models
	for (auto& iter : mModels)
		iter.second->Release();
	mModels.clear();

	// lights
	mDirectionalLights.clear();
	mLocalLights.clear();
	mGlobalLights.clear();

	// buffers
	mFinalBlurDepthBuffer.Shutdown();
	mHBlurDepthBuffer.Shutdown();
	mHDRBuffer.Shutdown();
	mDepthBuffer.Shutdown();
	mGBuffer.Shutdown();
	mSceneBuffer.Shutdown();

	mQuad.Shutdown();
}

void Renderer::Draw(float dt)
{
	mGraphics.GetPipeline()->BeginScene(XMFLOAT4(0, 0, 0, 1));
	mSceneBuffer.ClearRenderTarget();

	Render();

	mGraphics.GetPipeline()->SetBackFaceCull(true);
	mGraphics.GetPipeline()->SetZBuffer(true);
	mGraphics.GetPipeline()->EndScene();
}

//--------------------------------------------------------------------------------------
// Init Shaders
//--------------------------------------------------------------------------------------
HRESULT Renderer::InitShaders()
{
	HRESULT hr = S_OK;

	// 여러 버텍스 포맷을 다 컨트롤하지 말고 정해진 버텍스 포맷만 쓰게 만들어
	// 포즈, 노말, 탄젠트, 바이탄젠트
	// 텍스쳐가 있을지 없을지는 어떻게 온오프 해주지? 없으면 텍스쳐 좌표를 -1,-1로 해버리고 그걸로 거르자
	// 스키닝이야 따로 만들어야지
	// 일단 1로 바꾸고 나중에 모델들 전부 3 포맷으로 통일시켜

	// Geometry shaders
	mShaders["Geo"].Initialize(
		"Shader/VertexShaderLayout3.hlsl", "Shader/PixelShaderLayout3.hlsl",
		"GeometryVertexShader", "GeometryPixelShader");
	mShaders["Geo"].CreaterBuffer(PixelShader, 0, sizeof(MaterialBuffer));
	mShaders["Geo"].CreateSampler(PixelShader, 0);

	mShaders["Skinning"].Initialize(
		"Shader/VertexShaderLayout7.hlsl", "Shader/PixelShaderLayout7.hlsl",
		"SkinVertexShader", "SkinPixelShader");
	mShaders["Skinning"].CreaterBuffer(VertexShader, 1, sizeof(PaletteBuffer));

	// Ambient renders 
	//D3D_SHADER_MACRO ambientShaderMacros[3];
	//std::string piDef = std::to_string(Math::Pi);
	//std::string iblNDef = std::to_string(IBL_N);
	//ambientShaderMacros[0].Name = "PI";
	//ambientShaderMacros[0].Definition = piDef.c_str();
	//ambientShaderMacros[1].Name = "IBL_N";
	//ambientShaderMacros[1].Definition = iblNDef.c_str();
	//ambientShaderMacros[2].Name = NULL;
	//ambientShaderMacros[2].Definition = NULL;

	mShaders["Ambient"].Initialize(
		"Shader/AmbientVS.hlsl", "Shader/AmbientPS.hlsl",
		"AmbientVS", "AmbientPS");// ,
								  //nullptr,
								  //ambientShaderMacros);

								  // Ambient - Ambient buffer
	mShaders["Ambient"].CreaterBuffer(PixelShader, 0, sizeof(AmbientBuffer));
	//mShaders["Ambient"].CreaterBuffer(PixelShader, 1, sizeof(RandomBuffer));
	mShaders["Ambient"].CreateSampler(PixelShader, 0);

	// Lighting shaders
	std::string piDef = std::to_string(PI);
	D3D_SHADER_MACRO lightShaderMacros[2];
	lightShaderMacros[0].Name = "PI";
	lightShaderMacros[0].Definition = piDef.c_str();
	lightShaderMacros[1].Name = NULL;
	lightShaderMacros[1].Definition = NULL;

	mShaders["Lighting"].Initialize(
		"Shader/LightVS.hlsl", "Shader/LightPS.hlsl",
		"LightVS", "LightPS",
		nullptr,
		lightShaderMacros);
	mShaders["Lighting"].CreaterBuffer(PixelShader, 0, sizeof(LightBuffer));
	mShaders["Lighting"].CreaterBuffer(PixelShader, 1, sizeof(ShadowBuffer));
	mShaders["Lighting"].CreateSampler(PixelShader, 0);

	mShaders["DirLighting"].Initialize(
		"Shader/LightVS.hlsl", "Shader/DirectionalLightPS.hlsl",
		"LightVS", "DirectionalLightPS",
		nullptr,
		lightShaderMacros);
	mShaders["DirLighting"].CreateSampler(PixelShader, 0);

	// Depth shaders
	mShaders["Depth"].Initialize(
		"Shader/DepthVS.hlsl", "Shader/DepthPS.hlsl",
		"DepthVS", "DepthPS");

	mShaders["ExponentialDepth"].Initialize(
		"Shader/ExponentialDepthVS.hlsl", "Shader/ExponentialDepthPS.hlsl",
		"ExponentialDepthVS", "ExponentialDepthPS");
	mShaders["ExponentialDepth"].CreaterBuffer(PixelShader, 0, sizeof(ExponentialBuffer));

	// Blur shader macro
	std::string kernalRadiusDef = std::to_string(MAX_KERNEL_RADIUS);
	std::string batchXDef = std::to_string(BATCH_X_NUM);
	std::string batchYDef = std::to_string(BATCH_Y_NUM);

	D3D_SHADER_MACRO blurShaderMacros[4];
	blurShaderMacros[0].Name = "MAX_KERNEL_RADIUS";
	blurShaderMacros[0].Definition = kernalRadiusDef.c_str();
	blurShaderMacros[1].Name = "BATCH_X_NUM";
	blurShaderMacros[1].Definition = batchXDef.c_str();
	blurShaderMacros[2].Name = "BATCH_Y_NUM";
	blurShaderMacros[2].Definition = batchYDef.c_str();
	blurShaderMacros[3].Name = NULL;
	blurShaderMacros[3].Definition = NULL;

	mShaders["HBlur"].Initialize("Shader/HorizontalBlur.hlsl", "HorizontalBlurShader", blurShaderMacros);
	mShaders["HBlur"].CreaterBuffer(ComputeShader, 1, sizeof(XMFLOAT4) * (2 * MAX_KERNEL_RADIUS + 1));

	mShaders["VBlur"].Initialize("Shader/VerticalBlur.hlsl", "VerticalBlurShader", blurShaderMacros);
	mShaders["VBlur"].CreaterBuffer(ComputeShader, 1, sizeof(XMFLOAT4) * (2 * MAX_KERNEL_RADIUS + 1));

	// Screen Space Quad - Multiple World(Model) matrix only
	mShaders["SceneQuad"].Initialize(
		"Shader/QuadVS.hlsl", "Shader/QuadPS.hlsl",
		"QuadVS", "QuadPS");
	mShaders["SceneQuad"].CreateSampler(PixelShader, 0);

	return hr;
}

//--------------------------------------------------------------------------------------
// Init Camera
//--------------------------------------------------------------------------------------
HRESULT Renderer::InitCamera()
{
	HRESULT hr = S_OK;

	// Initialize the view matrix
	mCamera.UpdateView();

	// Initialize the projection matrix
	mPerspective = XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)Scr_Width / (float)Scr_Height, Near, Far);
	mOrthoMatrix = XMMatrixOrthographicLH((float)Scr_Width, (float)Scr_Height, Near, Far);

	return hr;
}

//--------------------------------------------------------------------------------------
// Init Light
//--------------------------------------------------------------------------------------
HRESULT Renderer::InitLight()
{
	// Init ambienet light
	mAmbientLight.SetColor(urColor::Salmon.ToVector4());

	//mGlobalLights.push_back(Light(urColor::White.ToVector4(), ursine::SVec3(50.f, 50.f, 0.f)));

	// Init local lights for diffuse and specular
	srand((unsigned int)time(nullptr));
	for (UINT i = 0; i < localLights; ++i)
	{
		ursine::SVec3 pos(
			(float)(rand() % 40) - 20.f,
			(float)(rand() % 10),
			(float)(rand() % 40) - 20.f);
		ursine::SVec4 randomColor(
			(float)(rand() % 255) / 255.f,
			(float)(rand() % 255) / 255.f,
			(float)(rand() % 255) / 255.f,
			-1.f);
		mLocalLights.push_back(LocalLight(pos, randomColor, 10.f));
	}
	
	mDirectionalLights.push_back(DirectionalLight(ursine::SVec3(0.f, 100.f, 0.f),
		urColor::Yellow.ToVector4(),
		ursine::SVec3(0.f, -1.f, 0.f),
		ursine::SVec3(1.f, 1.f, 1.f)));

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Initialize Models - 버텍스 리컨스트럭팅 바꿔야 함
//--------------------------------------------------------------------------------------
HRESULT Renderer::InitModels()
{
	HRESULT hr = S_OK;

	// Load fbx models in here
	// this is the place where fbx file loaded
	ursine::FBXModel* sphere = new ursine::FBXModel;
	hr = sphere->LoadFBX("Assets/Models/Sphere.fbx");
	FAIL_CHECK_WITH_MSG(hr, "Sphere.fbx load fail");

	ursine::FBXModel* model = new ursine::FBXModel;
	hr = model->LoadFBX("Assets/Models/MultiObj.fbx");
	FAIL_CHECK_WITH_MSG(hr, "Sphere.fbx load fail");

	mModels["Sphere"] = sphere;
	mModels["GeoModel"] = model;

	return hr;
}

//--------------------------------------------------------------------------------------
// Load HDR images
//--------------------------------------------------------------------------------------
HRESULT Renderer::InitHDR()
{
	HRESULT hr = S_OK;

	// IBL textures
	std::string hdr = "Assets/IBL/14-Hamarikyu_Bridge_B_3k.hdr";
	std::string radiance = "Assets/IBL/14-Hamarikyu_Bridge_B_3k.irr.hdr";

	//skyMap.Initialize(mDevice, deviceContext, hdr.c_str());
	//irradianceMap.Initialize(mDevice, deviceContext, radiance.c_str());

	return hr;
}

//--------------------------------------------------------------------------------------
// Update a frame
//--------------------------------------------------------------------------------------
void Renderer::Update(double deltaTime)
{
	if (mModels["GeoModel"])
		mModels["GeoModel"]->Update(deltaTime);
}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Renderer::Render()
{
	mCamera.UpdateView();

	//--------------------------------------------------------------------------------------
	// Deferred rendering
	//--------------------------------------------------------------------------------------
	// Pass1 - G Buffer Rendering. Render to texture
	// pos, norm, diff+transparency, spec+shineness, depth
	// Render Scene to the texture(render target)
	GeometryPass();
	// 이제 다시 지오 버퍼는 그려져. 이걸 씬 텍스쳐에다 그려서 디버그 윈도우로 출력해줘야 함

	////--------------------------------------------------------------------------------------
	//// IBL rendering
	////--------------------------------------------------------------------------------------
	//IBLPass();

	// Pass2 - Lighting Rendering
	// output to screen or final render target with viewport set to its width height
	// set blending off, depth testing off,
	// set ambi light amount
	// draw FSQ to invoke a pixel shader at all pixels
	// VS : pass the FSQ's vertex straight through. no transformation
	// PS : compute and output ambient light
	// Draw scene in buffer
	mSceneBuffer.SetRenderTarget();
	//AmbientPass();
	LightPass();

	// Set backbuffer as render target and clear
	mSceneBuffer.SetRenderTarget();
	mGraphics.GetPipeline()->SetBackBufferRenderTarget();
	DrawFinalScene();

#if DEBUG
#else
	//// Draw Text
	//WCHAR wstr[512];
	//mpSpriteBatch->Begin();
	//
	//// fps
	//mpFont->DrawString(mpSpriteBatch, L"FPS", XMFLOAT2(0, 0), DirectX::Colors::Yellow, 0, XMFLOAT2(0, 0), 0.5f);
	//swprintf_s(wstr, L"%d", frame_per_sec);
	//mpFont->DrawString(mpSpriteBatch, wstr, XMFLOAT2(0, 16), DirectX::Colors::Yellow, 0, XMFLOAT2(0, 0), 0.5f);
	//
	//// number of lights
	//mpFont->DrawString(mpSpriteBatch, L"Number of Lights", XMFLOAT2(45, 0), DirectX::Colors::Yellow, 0, XMFLOAT2(0, 0), 0.5f);
	//swprintf_s(wstr, L"%d", 2 + MAX_LIGHT);
	//mpFont->DrawString(mpSpriteBatch, wstr, XMFLOAT2(45, 16), DirectX::Colors::Yellow, 0, XMFLOAT2(0, 0), 0.5f);
	//
	//mpSpriteBatch->End();
#endif
}
