#pragma once

#include "UrsinePrecompiled.h"
#include <Color.h>
#include <Utilities.h>

using namespace DirectX;

const int MAX_LIGHT = 8;

namespace ursine
{
	//enum for the different types of lights
	enum LightType
	{
		LIGHT_NONE = -1,
		LIGHT_AMBIENT = 0,

		LIGHT_GLOBALDIRECTIONAL,
		LIGHT_GLOBALPOINT,
		LIGHT_GLOBALSPOTLIGHT,

		LIGHT_LOCALDIRECTIONAL,
		LIGHT_LOCALPOINT,
		LIGHT_LOCALSPOTLIGHT,

		LIGHT_COUNT
	};

	///////////////////////////////////////////////////////////////
	//// LIGHT ////////////////////////////////////////////////////
	class Light
	{
	public:
		Light();
		Light(const Light&);
		~Light();

		void Initialize(XMFLOAT4 color, ursine::LightType type, XMFLOAT3 dir, XMFLOAT3 pos);
		void Reset();
		LightType GetType(void) const { return m_type; }
		void SetType(const LightType type) { m_type = type; }
		
		XMFLOAT3 GetPosition() const { return m_position; }
		void SetPosition(XMFLOAT3 pos) { m_position = pos; }
		void SetPosition(float x, float y, float z) { m_position = XMFLOAT3(x, y, z); }
		float GetRotation() { return m_rotation; }
		void SetRotation(float rot) { m_rotation = rot; }

		void SetRange(float range) { m_range = range; }
		void SetColor(float red, float green, float blue, float alpha) { m_color = XMFLOAT4(red, green, blue, alpha); }
		void SetColor(XMFLOAT4 rgba) { m_color = rgba; }
		void SetDirection(float x, float y, float z) { m_direction = XMFLOAT3(x, y, z); }
		
		XMFLOAT4 GetColor() const { return m_color; }
		float GetRange() const { return m_range; }
		XMFLOAT3 GetDirection() const { return m_direction; }
		XMMATRIX GetTransformation() const { return m_transfrom; }
		XMMATRIX GetShadowView() const { return m_view; }
		XMMATRIX GetShadowProjection() const { return m_proj; }

		void Update();
		void GenerateShadowView(void);
		void GenerateShadowProjection(float fov, float aspect, float lightnear, float lightfar);

	private:
		LightType m_type;
		XMFLOAT3 m_position;
		float m_rotation;
		XMFLOAT4 m_color;
		float m_range;
		XMFLOAT3 m_direction;
		XMMATRIX m_transfrom;
		XMMATRIX m_view;
		XMMATRIX m_proj;

		// initial value
		XMFLOAT3 initPos;
		float	initRot;
	};

	//class Light
	//{
	//public:
	//	//enum for the different types of lights
	//	enum LightType
	//	{
	//		LIGHT_DIRECTIONAL = 0,
	//		LIGHT_POINT,
	//		LIGHT_SPOTLIGHT,
	//		LIGHT_COUNT
	//	};
	//
	//public:
	//	void Initialize(void);
	//
	//	LightType GetType(void);
	//	void SetType(const LightType type);
	//
	//	const SVec3 &GetDirection(void);
	//	void SetDirection(const SVec3 &dir);
	//	void SetDirection(const float x, const float y, const float z);
	//
	//	const SVec3 &GetPosition(void);
	//	void SetPosition(const SVec3 &position);
	//	void SetPosition(const float x, const  float y, const float z);
	//
	//	const Color &GetColor(void);
	//	void SetColor(const Color &color);
	//	void SetColor(const float r, const float g, const float b);
	//
	//	float GetRadius(void);
	//	void SetRadius(const float radius);
	//
	//	float GetIntensity(void);
	//	void SetIntensity(const float intensity);
	//
	//	const Vec2 &GetSpotlightAngles(void);
	//	void SetSpotlightAngles(const Vec2 &angles);
	//	void SetSpotlightAngles(const float inner, const float outer);
	//
	//	void SetSpotlightTransform(const SMat4 &transf);
	//	const SMat4 &GetSpotlightTransform(void) const;
	//
	//	//SMat4 GenerateViewSpaceShadowTransform(void) const;
	//	//SMat4 GenerateViewSpaceShadowProjection(void) const;
	//	//
	//	//SMat4 GenerateShadowView(void) const;
	//	//SMat4 GenerateShadowProjection(void) const;
	//	//
	//	//unsigned GetShadowmapWidth(void) const;
	//	//void SetShadowmapWidth(unsigned width);
	//	//
	//	//GfxHND GetShadowmapHandle(void) const;
	//	//void SetShadowmapHandle(GfxHND handle);
	//	//
	//	//bool GetRenderShadows(void) const;
	//	//void SetRenderShadows(bool renderShadows);
	//
	//	Light &operator=(Light &rhs);
	//
	//private:
	//	LightType m_type;
	//	SVec3 m_position;
	//	Color m_color;
	//	float m_radius;
	//	SVec3 m_direction;
	//	float m_intensity;
	//
	//	Vec2 m_spotlightAngles;
	//	SMat4 m_spotlightTransform;
	//
	//	//unsigned m_shadowmapWidth;
	//	//GfxHND m_shadowmap;
	//	//bool m_renderShadows;
	//};
}