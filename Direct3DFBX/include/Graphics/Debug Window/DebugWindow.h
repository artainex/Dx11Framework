#pragma once

#include <Utilities.h>

using namespace DirectX;

class DebugWindow
{
private:
	struct VertexType
	{
		XMVECTOR position;
		XMVECTOR texture;

		VertexType()
		{
			position.m128_f32[0] = 0;
			position.m128_f32[1] = 0;
			position.m128_f32[2] = 0;
			position.m128_f32[3] = 0;

			texture.m128_f32[0] = 0;
			texture.m128_f32[1] = 0;
			texture.m128_f32[2] = 0;
			texture.m128_f32[3] = 0;
		}
	};

public:
	DebugWindow();
	DebugWindow(const DebugWindow&);
	~DebugWindow();

	bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY);
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
};

