////////////////////////////////////////////////////////////////////////////////
// Filename: Cameraclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <directxmath.h>
#include <SMat4.h>

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class Camera
{
public:
	// Constructor
	Camera();

	// Destructor
	~Camera();

	// Set Positiom
	void SetPosition(const ursine::SVec3 & position);

	// Set Target or Look At
	void SetTarget(const ursine::SVec3 & lookat);

	// Set up vector
	void SetUp(const ursine::SVec3 & up);

	// Get Position
	ursine::SVec3 GetPosition() const;

	// Get Look At
	ursine::SVec3 GetTarget() const;

	// Get Up
	ursine::SVec3 GetUp() const;

	// Get view matrix
	XMMATRIX GetView() const;

	// Update view matrix
	void UpdateView();

private:
	XMMATRIX mView;		//!< View Matrix

	ursine::SVec3 mPosition;	//!< Camera Position
	ursine::SVec3 mTarget;	//!< Camera Look at
	ursine::SVec3 mUp;		//!< Camera Up

	bool mUpdate;		//!< Flag to check if update is needed
};