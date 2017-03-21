#include <Light.h>

///////////////////////////////////////////////////////////////////
//lights
Light::Light()
	: 
	mPos(0, 0, 0),
	mColor(0, 0, 0, 0)
{
	mView = XMMatrixIdentity();
	mProj = XMMatrixIdentity();
}

Light::Light(ursine::SVec4 color, ursine::SVec3 pos)
	: mPos(pos), mColor(color)
{
}

Light::~Light()
{
}

void Light::Initialize(ursine::SVec4 color, ursine::SVec3 pos)
{
	mPos = pos;
	mColor = color;
}

void Light::Reset()
{
	//m_position = initPos;
}

void Light::Update()
{
	//ursine::SMat4 transl = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	//
	//ursine::Vec4 yaxis = ursine::Vec4(0, 1, 0, 1);
	//ursine::SMat4 rot = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat4(&yaxis), m_rotation);
	//m_transfrom = transl * rot;
	//
	//ursine::Vec4 newPos;
	//DirectX::XMStoreFloat4(&newPos, m_transfrom.r[3]);
	//
	//m_position = ursine::Vec3(newPos.x, newPos.y, newPos.z);
	//m_direction = ursine::Vec3(-newPos.x, -newPos.y, -newPos.z);
}

void Light::GenerateShadowView(const ursine::SVec3 & target, const ursine::SVec3 & up)
{
	DirectX::XMVECTOR eyev		= DirectX::XMLoadFloat3( &XMFLOAT3( mPos.X(), mPos.Y(), mPos.Z() ) );
	DirectX::XMVECTOR targetv	= DirectX::XMLoadFloat3( &XMFLOAT3( target.X(), target.Y(), target.Z() ) );
	DirectX::XMVECTOR upv		= DirectX::XMLoadFloat3( &XMFLOAT3( up.X(), up.Y(), up.Z() ) );

	mView = XMMatrixLookAtLH(eyev, targetv, upv);
}

void Light::GenerateShadowProjection(float fov, float aspect, float lightnear, float lightfar)
{
	mProj = XMMatrixPerspectiveFovLH(fov, aspect, lightnear, lightfar);
}

void LocalLight::Update()
{
}

void LocalLight::GenerateShadowView(const ursine::SVec3 & target, const ursine::SVec3 & up)
{
	DirectX::XMVECTOR eyev = DirectX::XMLoadFloat3(&XMFLOAT3(mPos.X(), mPos.Y(), mPos.Z()));
	DirectX::XMVECTOR targetv = DirectX::XMLoadFloat3(&XMFLOAT3(target.X(), target.Y(), target.Z()));
	DirectX::XMVECTOR upv = DirectX::XMLoadFloat3(&XMFLOAT3(up.X(), up.Y(), up.Z()));

	mView = XMMatrixLookAtLH(eyev, targetv, upv);
}

void LocalLight::GenerateShadowProjection(float fov, float aspect, float lightnear, float lightfar)
{
	mProj = XMMatrixPerspectiveFovLH(fov, aspect, lightnear, lightfar);
}

void DirectionalLight::GenerateShadowView(const ursine::SVec3 & target, const ursine::SVec3 & up)
{
	DirectX::XMVECTOR eyev = DirectX::XMLoadFloat3(&XMFLOAT3(mPos.X(), mPos.Y(), mPos.Z()));
	DirectX::XMVECTOR targetv = DirectX::XMLoadFloat3(&XMFLOAT3(target.X(), target.Y(), target.Z()));
	DirectX::XMVECTOR upv = DirectX::XMLoadFloat3(&XMFLOAT3(up.X(), up.Y(), up.Z()));

	mView = XMMatrixLookAtLH(eyev, targetv, upv);
}

void DirectionalLight::GenerateShadowProjection(float fov, float aspect, float lightnear, float lightfar)
{
	mProj = XMMatrixPerspectiveFovLH(fov, aspect, lightnear, lightfar);
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