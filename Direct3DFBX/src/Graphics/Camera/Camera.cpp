////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include <Camera.h>

Camera::Camera()
{
	mPos.m128_f32[0] = 0.f;
	mPos.m128_f32[1] = 0.f;
	mPos.m128_f32[2] = 0.f;
	mPos.m128_f32[3] = 1.f;

	mRot.m128_f32[0] = 0.f;
	mRot.m128_f32[1] = 0.f;
	mRot.m128_f32[2] = 0.f;
	mRot.m128_f32[3] = 1.f;
}

Camera::Camera(const Camera& other)
{
	if ( this != &other )
	{
		mPos = other.mPos;
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
	eyePos = mPos;

	// Setup where the camera is looking by default.
	lookAt.m128_f32[0] = 0.0f;
	lookAt.m128_f32[1] = 0.0f;
	lookAt.m128_f32[2] = 1.0f;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch	= mRot.m128_f32[0] * 0.0174532925f;
	yaw		= mRot.m128_f32[1] * 0.0174532925f;
	roll	= mRot.m128_f32[2] * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotMtx = XMMatrixRotationRollPitchYaw(yaw, pitch, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAt	= XMVector3TransformCoord(lookAt, rotMtx);
	up		= XMVector3TransformCoord(up, rotMtx);

	// Translate the rotated camera position to the location of the viewer.
	//lookAt = eyePos + lookAt;

	// Finally create the view matrix from the three updated vectors.
	mViewMtx = XMMatrixLookAtLH(eyePos, lookAt, up);

	return;
}