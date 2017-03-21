#pragma once

#include <DirectXMath.h>
#include <SVec3.h>
#include <SVec4.h>

using namespace DirectX;

///////////////////////////////////////////////////////////////
//// LIGHT ////////////////////////////////////////////////////
class Light
{
public:
	Light();
	Light(ursine::SVec4 color, ursine::SVec3 pos);
	~Light();

	void Initialize(ursine::SVec4 color, ursine::SVec3 pos);
	void Reset();

	ursine::SVec3 GetPosition() const { return mPos; }
	void SetPosition(ursine::SVec3 pos) { mPos = pos; }
	void SetPosition(float x, float y, float z) { mPos = ursine::SVec3(x, y, z); }

	void SetColor(float red, float green, float blue, float alpha) { mColor = ursine::SVec4(red, green, blue, alpha); }
	void SetColor(ursine::SVec4 rgba) { mColor = rgba; }

	const ursine::SVec4& GetColor() const { return mColor; }
	const XMMATRIX& GetShadowView() const { return mView; }
	const XMMATRIX& GetShadowProjection() const { return mProj; }

	void Update();
	void GenerateShadowView(const ursine::SVec3 & target, const ursine::SVec3 & up);
	void GenerateShadowProjection(float fov, float aspect, float lightnear, float lightfar);

private:
	ursine::SVec3 mPos;
	ursine::SVec4 mColor;
	XMMATRIX mView;
	XMMATRIX mProj;
};

// 잘 작동되면 상속으로 바꿀것
class LocalLight
{
public:
	// Constructor
	LocalLight()
		: mPos(0.f, 0.f, 0.f), mColor(0.f, 0.f, 0.f, 0.f), mRadius(0.f) {}
	LocalLight(const ursine::SVec3 & position, const ursine::SVec4 & color, float radius)
		: mPos(position), mColor(color), mRadius(radius) {}

	// Destructor
	~LocalLight() {};

	ursine::SVec3 GetPosition() const { return mPos; }
	void SetPosition(ursine::SVec3 pos) { mPos = pos; }
	void SetPosition(float x, float y, float z) { mPos = ursine::SVec3(x, y, z); }

	float GetRadius() const { return mRadius; }
	void SetRadius(float rad) { mRadius = rad; }

	void SetColor(float red, float green, float blue, float alpha) { mColor = ursine::SVec4(red, green, blue, alpha); }
	void SetColor(ursine::SVec4 rgba) { mColor = rgba; }

	const ursine::SVec4& GetColor() const { return mColor; }
	const XMMATRIX& GetShadowView() const { return mView; }
	const XMMATRIX& GetShadowProjection() const { return mProj; }

	void Update();
	void GenerateShadowView(const ursine::SVec3 & target, const ursine::SVec3 & up);
	void GenerateShadowProjection(float fov, float aspect, float lightnear, float lightfar);

private:
	ursine::SVec3 mPos;
	ursine::SVec4 mColor;
	float mRadius;
	XMMATRIX mView;
	XMMATRIX mProj;
};

class DirectionalLight
{
public:
	DirectionalLight()
		: mPos(0.f, 0.f, 0.f), mColor(0.f, 0.f, 0.f, 0.f), 
		mDirection(0.f, 0.f, 1.f), mDimension(0.f, 0.f, 0.f)
	{}

	// Constructor
	DirectionalLight(const ursine::SVec3 & position, const ursine::SVec4 & color,
					const ursine::SVec3 & direction, const ursine::SVec3 & dimension)
		: mPos(position), mColor(color), mDirection(direction) , mDimension(dimension)
	{
		mDirection.Normalize();
	}

	// Destructor
	~DirectionalLight() {};

	ursine::SVec3 GetPosition() const { return mPos; }
	void SetPosition(ursine::SVec3 pos) { mPos = pos; }
	void SetPosition(float x, float y, float z) { mPos = ursine::SVec3(x, y, z); }

	ursine::SVec3 GetDirection() const { return mDirection; }
	void SetDirection(ursine::SVec3 dir) { mDirection = dir; }
	void SetDirection(float x, float y, float z) { mDirection = ursine::SVec3(x, y, z); }

	ursine::SVec3 GetDimension() const { return mDimension; }
	void SetDimension(ursine::SVec3 dim) { mDimension = dim; }
	void SetDimension(float x, float y, float z) { mDimension = ursine::SVec3(x, y, z); }

	void SetColor(float red, float green, float blue, float alpha) { mColor = ursine::SVec4(red, green, blue, alpha); }
	void SetColor(ursine::SVec4 rgba) { mColor = rgba; }
	const ursine::SVec4& GetColor() const { return mColor; }

	const XMMATRIX& GetShadowView() const { return mView; }
	const XMMATRIX& GetShadowProjection() const { return mProj; }

	void GenerateShadowView(const ursine::SVec3 & target, const ursine::SVec3 & up);
	void GenerateShadowProjection(float fov, float aspect, float lightnear, float lightfar);

private:
	ursine::SVec3 mPos;
	ursine::SVec3 mDirection;
	ursine::SVec3 mDimension;	// width, height, depth
	ursine::SVec4 mColor;
	XMMATRIX mView;
	XMMATRIX mProj;
};