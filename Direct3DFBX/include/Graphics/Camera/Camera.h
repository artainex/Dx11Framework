////////////////////////////////////////////////////////////////////////////////
// Filename: Cameraclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <WholeInformation.h>

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class Camera
{
public:
	Camera();
	Camera(const Camera&);
	~Camera();

	void Update();

	void SetPosition(const XMVECTOR& pos) { mPos = pos; }
	void SetRotation(const XMVECTOR& rot) { mRot = rot; }

	const XMVECTOR& GetPosition() const { return mPos; }
	const XMVECTOR& GetRotation() const { return mRot; }

	void SetViewMatrix();
	void SetViewMatrix(const XMMATRIX& viewMtx) { mViewMtx = viewMtx; }
	const XMMATRIX& GetViewMatrix() const { return mViewMtx; }

private:
	XMVECTOR mPos;
	XMVECTOR mRot;
	XMMATRIX mViewMtx;
};