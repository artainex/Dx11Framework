#include <Light.h>

namespace ursine
{
	///////////////////////////////////////////////////////////////////
	//lights
	Light::Light()
		: m_type(LIGHT_NONE),
		m_position(XMFLOAT3(0, 0, 0)),
		m_rotation(0.f),
		m_color(XMFLOAT4(0, 0, 0, 0)),
		m_range(500.f),
		m_direction(XMFLOAT3(0, 0, 0))
	{
		m_transfrom = XMMatrixIdentity();
		m_view = XMMatrixIdentity();
		m_proj = XMMatrixIdentity();
	}


	Light::Light(const Light& other)
		: m_type(LIGHT_NONE)
	{
	}


	Light::~Light()
	{
	}

	void Light::Initialize(XMFLOAT4 color, ursine::LightType type, XMFLOAT3 dir, XMFLOAT3 pos)
	{
		m_color = color;
		m_type = type;
		m_direction = dir;
		m_position = initPos = pos;
	}

	void Light::Reset()
	{
		m_position = initPos;
	}

	void Light::Update()
	{
		XMMATRIX transl = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

		XMFLOAT4 yaxis = XMFLOAT4(0, 1, 0, 1);
		XMMATRIX rot = XMMatrixRotationAxis(XMLoadFloat4(&yaxis), m_rotation);
		m_transfrom = transl * rot;

		XMFLOAT4 newPos;
		XMStoreFloat4(&newPos, m_transfrom.r[3]);

		m_position = XMFLOAT3(newPos.x, newPos.y, newPos.z);

		GenerateShadowView();
	}

	void Light::GenerateShadowView(void)
	{
		auto tempPos = m_position;
		auto position = DirectX::XMFLOAT4( tempPos.x, tempPos.y, tempPos.z, 1.0f);
	
		auto tempLook = m_direction;
		auto dir = DirectX::XMFLOAT4( tempLook.x, tempLook.y, tempLook.z, 0.0f);
	
		auto up = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f );

		m_view = XMMatrixLookToLH(
			XMLoadFloat4(&position),
			XMLoadFloat4(&dir),
			XMLoadFloat4(&up));
	}
	
	void Light::GenerateShadowProjection(float fov, float aspect, float lightnear, float lightfar)
	{
		m_proj = XMMatrixPerspectiveFovLH(fov, aspect, lightnear, lightfar);
	}

	/////////////////////////////////////////////////////////////////////
	//// universal light
	//void Light::Initialize(void)
	//{
	//	m_type = LIGHT_DIRECTIONAL;
	//	m_position = SVec3(0, 0, 0);
	//	m_color = Color::White;
	//	m_radius = 1.0f;
	//	m_direction = SVec3(0, -1, 0);
	//	m_intensity = 1.0f;;
	//
	//	m_spotlightAngles = Vec2(15, 30);
	//
	//	//m_shadowmap = 0;
	//	//m_shadowmapWidth = 1024;
	//	//m_renderShadows = false;
	//}
	//
	//const SVec3& Light::GetDirection(void)
	//{
	//	return m_direction;
	//}
	//
	//void Light::SetDirection(const SVec3& dir)
	//{
	//	m_direction = dir;
	//}
	//
	//void Light::SetDirection(const float x, const float y, const float z)
	//{
	//	m_direction = SVec3(x, y, z);
	//}
	//
	//const SVec3& Light::GetPosition(void)
	//{
	//	return m_position;
	//}
	//
	//void Light::SetPosition(const SVec3& position)
	//{
	//	m_position = position;
	//}
	//
	//void Light::SetPosition(const float x, const float y, const float z)
	//{
	//	m_position = SVec3(x, y, z);
	//}
	//
	//const Color& Light::GetColor(void)
	//{
	//	return m_color;
	//}
	//
	//void Light::SetColor(const Color& color)
	//{
	//	m_color = color;
	//}
	//
	//void Light::SetColor(const float r, const float g, const  float b)
	//{
	//	m_color = Color(r, g, b, 1.0f);
	//}
	//
	//float Light::GetRadius(void)
	//{
	//	return m_radius;
	//}
	//
	//void Light::SetRadius(const float radius)
	//{
	//	m_radius = radius;
	//}
	//
	//float Light::GetIntensity(void)
	//{
	//	return m_intensity;
	//}
	//
	//void Light::SetIntensity(const float intensity)
	//{
	//	m_intensity = intensity;
	//}
	//
	//const Vec2& Light::GetSpotlightAngles(void)
	//{
	//	return m_spotlightAngles;
	//}
	//
	//void Light::SetSpotlightAngles(const Vec2& angles)
	//{
	//	m_spotlightAngles = angles;
	//}
	//
	//void Light::SetSpotlightAngles(const float inner, const float outer)
	//{
	//	m_spotlightAngles = Vec2(inner, outer);
	//}
	//
	//void Light::SetSpotlightTransform(const SMat4& transf)
	//{
	//	m_spotlightTransform = transf;
	//}
	//
	//const SMat4& Light::GetSpotlightTransform() const
	//{
	//	return m_spotlightTransform;
	//}
	//
	//
	//unsigned Light::GetShadowmapWidth(void) const
	//{
	//	return m_shadowmapWidth;
	//}
	//
	//void Light::SetShadowmapWidth(unsigned width)
	//{
	//	m_shadowmapWidth = width;
	//}
	//
	//GfxHND Light::GetShadowmapHandle(void) const
	//{
	//	return m_shadowmap;
	//}
	//
	//void Light::SetShadowmapHandle(GfxHND handle)
	//{
	//	m_shadowmap = handle;
	//}
	//
	//bool Light::GetRenderShadows(void) const
	//{
	//	return m_renderShadows;
	//}
	//
	//void Light::SetRenderShadows(bool renderShadows)
	//{
	//	m_renderShadows = renderShadows;
	//}

//#define COPY(name) name = rhs.name;
//
//	Light &Light::operator=(Light &rhs)
//	{
//		COPY(m_type);
//		COPY(m_position);
//		COPY(m_color);
//		COPY(m_radius);
//		COPY(m_direction);
//		COPY(m_intensity);
//		COPY(m_spotlightAngles);
//		COPY(m_spotlightTransform);
//		
//		//COPY(m_renderShadows);
//		//
//		//// old one will have old handle
//		//COPY(m_shadowmapWidth);
//		//
//		//rhs.m_shadowmap = m_shadowmap;
//
//		return *this;
//	}
}