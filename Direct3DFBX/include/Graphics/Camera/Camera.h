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

	void SetPosition(const XMFLOAT3& pos) { mPos = pos; }
	const XMFLOAT3& GetPosition() const { return mPos; }

	void SetRotation(const XMFLOAT3& rot) { mRot = rot; }
	const XMFLOAT3& GetRotation() const { return mRot; }

	void SetLookAt(const XMFLOAT3& lookat) { mLookAt = lookat; }
	const XMFLOAT3& GetLookAt() const { return mLookAt; }

	void SetUp(const XMFLOAT3& up) { mUp = up; }
	const XMFLOAT3& GetUp() const { return mUp; }

	void SetViewMatrix();
	void SetViewMatrix(const XMMATRIX& viewMtx) { mViewMtx = viewMtx; }
	const XMMATRIX& GetViewMatrix() const { return mViewMtx; }

private:
	XMFLOAT3 mPos;
	XMFLOAT3 mRot;
	XMFLOAT3 mLookAt;
	XMFLOAT3 mUp;
	XMMATRIX mViewMtx;
};