#include <Graphics.h>

Graphics Graphics::sInstance;

Graphics::Graphics()
{
}

Graphics::~Graphics()
{
}

bool Graphics::Initialize(const PlatformWindow& window, float near_dist, float far_dist)
{
	bool result = mPipeline.Initialize(window.GetWidth(),
		window.GetHeight(),
		window.mHwnd,
		near_dist,
		far_dist,
		window.GetFullscreen());

	if (!result)
	{
		MessageBox(window.mHwnd, "Could not initialize Direct3D.", "Error", MB_OK);
		return false;
	}

	return true;
}

void Graphics::Shutdown()
{
	mPipeline.Shutdown();
}

GraphicsPipeline * Graphics::GetPipeline()
{
	return &mPipeline;
}