////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include <Camera.h>

Camera::Camera()
{
	mPos = XMFLOAT3(0.f, 0.f, -10.f);
	mRot = XMFLOAT3(0.f, 0.f, 0.f);
	mUp = XMFLOAT3(0.f, 1.f, 0.f);
}

Camera::Camera(const Camera& other)
{
	if ( this != &other )
	{
		mPos = other.mPos;
		mRot = other.mRot;
		mUp = other.mUp;
		mRot = other.mRot;
		mViewMtx = other.mViewMtx;
	}
}

Camera::~Camera()
{
}

void Camera::Update()
{
	SetViewMatrix();
}

void Camera::SetViewMatrix()
{
	XMVECTOR up, eyePos, lookAt;
	float yaw, pitch, roll;
	XMMATRIX rotMtx = XMMatrixIdentity();

	// Setup the vector that points upwards.
	up.m128_f32[0] = 0.0f;
	up.m128_f32[1] = 1.0f;
	up.m128_f32[2] = 0.0f;

	// Setup the position of the camera in the world.
	eyePos.m128_f32[0] = mPos.x;
	eyePos.m128_f32[1] = mPos.y;
	eyePos.m128_f32[2] = mPos.z;

	// Setup where the camera is looking by default.
	lookAt.m128_f32[0] = mLookAt.x;
	lookAt.m128_f32[1] = mLookAt.y;
	lookAt.m128_f32[2] = mLookAt.z;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = mRot.x * 0.0174532925f;
	yaw = mRot.y * 0.0174532925f;
	roll = mRot.z * 0.0174532925f;

	// Create the rotation matrix from the pitch, yaw and roll values.
	rotMtx = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAt = XMVector3TransformCoord(lookAt, rotMtx);
	up = XMVector3TransformCoord(up, rotMtx);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = eyePos + lookAt;

	// Finally create the view matrix from the three updated vectors.
	mViewMtx = XMMatrixLookAtLH(eyePos, lookAt, up);

	return;
}