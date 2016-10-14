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
		LIGHT_DIRECTIONAL = 0,
		LIGHT_POINT,
		LIGHT_SPOTLIGHT,
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

		LightType GetType(void) const { return m_type; }
		void SetType(const LightType type) { m_type = type; }

		XMFLOAT3 GetPosition() const { return m_position; }
		void SetPosition(XMFLOAT3 pos) { m_position = pos; m_transfrom = XMMatrixTranslation(m_position.x, m_position.y, m_position.z); }
		void SetPosition(float x, float y, float z) { m_position = XMFLOAT3(x, y, z); m_transfrom = XMMatrixTranslation(m_position.x, m_position.y, m_position.z); }

		void SetDiffuseColor(float red, float green, float blue, float alpha) { m_diffuseColor = XMFLOAT4(red, green, blue, alpha); }
		void SetAmbientColor(float red, float green, float blue, float alpha) { m_ambientColor = XMFLOAT4(red, green, blue, alpha); }
		void SetSpecularColor(float red, float green, float blue, float alpha) { m_specularColor = XMFLOAT4(red, green, blue, alpha); }
		void SetDiffuseColor(XMFLOAT4 rgba) { m_diffuseColor = rgba; }
		void SetAmbientColor(XMFLOAT4 rgba) { m_ambientColor = rgba; }
		void SetSpecularColor(XMFLOAT4 rgba) { m_specularColor = rgba; }
		void SetDirection(float x, float y, float z) { m_direction = XMFLOAT3(x, y, z); }

		XMFLOAT4 GetDiffuseColor() const { return m_diffuseColor; }
		XMFLOAT4 GetAmbientColor() const { return m_ambientColor; }
		XMFLOAT4 GetSpecularColor() const { return m_specularColor; }
		XMFLOAT3 GetDirection() const { return m_direction; }

		XMMATRIX GetTransformation() const { return m_transfrom; }

	private:
		LightType m_type;
		XMFLOAT3 m_position;
		XMFLOAT4 m_diffuseColor;
		XMFLOAT4 m_ambientColor;
		XMFLOAT4 m_specularColor;
		XMFLOAT3 m_direction;
		XMMATRIX m_transfrom;
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