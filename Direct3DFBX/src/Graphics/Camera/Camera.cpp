////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include <Camera.h>

Camera::Camera()
	: mPosition(0.f, 40.0f, -40.0f)
	, mTarget(0.0f, 0.0f, 0.0f)
	, mUp(0.0f, 1.0f, 0.0f)
	, mUpdate(true)
{
}

Camera::~Camera()
{
}

void Camera::SetPosition(const ursine::SVec3 & position)
{
	mPosition = position;
	mUpdate = true;
}

void Camera::SetTarget(const ursine::SVec3 & lookat)
{
	mTarget = lookat;
	mUpdate = true;
}

void Camera::SetUp(const ursine::SVec3 & up)
{
	mUp = up;
	mUpdate = true;
}

ursine::SVec3 Camera::GetPosition() const
{
	return mPosition;
}

ursine::SVec3 Camera::GetTarget() const
{
	return mTarget;
}

ursine::SVec3 Camera::GetUp() const
{
	return mUp;
}

XMMATRIX Camera::GetView() const
{
	return mView;
}

void Camera::UpdateView()
{
	if (!mUpdate)
		return;

	//mView = ursine::SMat4::LookAt( mPosition, mTarget, ursine::SVec3::UnitZ(), mUp, ursine::SVec3::UnitY() );

	XMVECTOR pos = XMLoadFloat3(&XMFLOAT3(mPosition.X(), mPosition.Y(), mPosition.Z()));
	XMVECTOR target = XMLoadFloat3(&XMFLOAT3(mTarget.X(), mTarget.Y(), mTarget.Z()));
	XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0.f, 1.f, 0.f));

	mView = XMMatrixLookAtLH(pos, target, up);
	mUpdate = false;
}
//void Camera::SetViewMatrix()
//{
//	XMVECTOR up, eyePos, lookAt;
//	float yaw, pitch, roll;
//	XMMATRIX rotMtx = XMMatrixIdentity();
//
//	// Setup the vector that points upwards.
//	up.m128_f32[0] = 0.0f;
//	up.m128_f32[1] = 1.0f;
//	up.m128_f32[2] = 0.0f;
//
//	// Setup the position of the camera in the world.
//	eyePos.m128_f32[0] = mPos.x;
//	eyePos.m128_f32[1] = mPos.y;
//	eyePos.m128_f32[2] = mPos.z;
//
//	// Setup where the camera is looking by default.
//	lookAt.m128_f32[0] = mLookAt.x;
//	lookAt.m128_f32[1] = mLookAt.y;
//	lookAt.m128_f32[2] = mLookAt.z;
//
//	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
//	pitch = mRot.x * 0.0174532925f;
//	yaw = mRot.y * 0.0174532925f;
//	roll = mRot.z * 0.0174532925f;
//
//	// Create the rotation matrix from the pitch, yaw and roll values.
//	rotMtx = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
//
//	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
//	lookAt = XMVector3TransformCoord(lookAt, rotMtx);
//	up = XMVector3TransformCoord(up, rotMtx);
//
//	// Translate the rotated camera position to the location of the viewer.
//	lookAt = eyePos + lookAt;
//
//	// Finally create the view matrix from the three updated vectors.
//	mViewMtx = XMMatrixLookAtLH(eyePos, lookAt, up);
//
//	return;
//}