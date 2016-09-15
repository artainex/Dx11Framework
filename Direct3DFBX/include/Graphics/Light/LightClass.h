#pragma once

#include "UrsinePrecompiled.h"
#include <Color.h>

namespace ursine
{
	/////////////////////////////////////////////////////////////
	// LIGHT ////////////////////////////////////////////////////
	class Light
	{
	public:
		//enum for the different types of lights
		enum LightType
		{
			LIGHT_DIRECTIONAL = 0,
			LIGHT_POINT,
			LIGHT_SPOTLIGHT,

			LIGHT_COUNT
		};

	public:
		void Initialize(void);

		LightType GetType(void);
		void SetType(const LightType type);

		const SVec3 &GetDirection(void);
		void SetDirection(const SVec3 &dir);
		void SetDirection(const float x, const float y, const float z);

		const SVec3 &GetPosition(void);
		void SetPosition(const SVec3 &position);
		void SetPosition(const float x, const  float y, const float z);

		const Color &GetColor(void);
		void SetColor(const Color &color);
		void SetColor(const float r, const float g, const float b);

		float GetRadius(void);
		void SetRadius(const float radius);

		float GetIntensity(void);
		void SetIntensity(const float intensity);

		const Vec2 &GetSpotlightAngles(void);
		void SetSpotlightAngles(const Vec2 &angles);
		void SetSpotlightAngles(const float inner, const float outer);

		void SetSpotlightTransform(const SMat4 &transf);
		const SMat4 &GetSpotlightTransform(void) const;

		//SMat4 GenerateViewSpaceShadowTransform(void) const;
		//SMat4 GenerateViewSpaceShadowProjection(void) const;
		//
		//SMat4 GenerateShadowView(void) const;
		//SMat4 GenerateShadowProjection(void) const;
		//
		//unsigned GetShadowmapWidth(void) const;
		//void SetShadowmapWidth(unsigned width);
		//
		//GfxHND GetShadowmapHandle(void) const;
		//void SetShadowmapHandle(GfxHND handle);
		//
		//bool GetRenderShadows(void) const;
		//void SetRenderShadows(bool renderShadows);

		Light &operator=(Light &rhs);

	private:
		LightType m_type;
		SVec3 m_position;
		Color m_color;
		float m_radius;
		SVec3 m_direction;
		float m_intensity;

		Vec2 m_spotlightAngles;
		SMat4 m_spotlightTransform;

		//unsigned m_shadowmapWidth;
		//GfxHND m_shadowmap;
		//bool m_renderShadows;
	};
}