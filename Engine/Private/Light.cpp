#include "Light.h"
//#include "Shader.h"

CLight::CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{
}

CLight::CLight(const CLight& Prototype)
	:	CComponent{ Prototype }
	,	m_tInfo{ Prototype.m_tInfo }
{
}

HRESULT CLight::Initialize_Prototype(LIGHTTYPE eType)
{
	m_tInfo.eType			= eType; // 빛 타입은 이미 설정되어 있다고 가정
	m_tInfo.fDiffuse		= { 1.0f, 1.0f, 1.0f, 1.0f }; // 흰색 확산광
	m_tInfo.fAmbient		= { 0.2f, 0.2f, 0.2f, 1.0f }; // 약한 주변광
	m_tInfo.fSpecular		= { 1.0f, 1.0f, 1.0f, 1.0f }; // 흰색 반사광
	m_tInfo.fPosition		= { 0.0f, 10.0f, 0.0f }; // 빛의 위치 (위에서 아래로 비추는 방향을 가정)
	m_tInfo.fDirection		= { 0.0f, -1.0f, 0.0f }; // 빛의 방향 (아래쪽)
	m_tInfo.fAttenuation0	= 1.0f; // 감쇠 상수
	m_tInfo.fAttenuation1	= 0.0f; // 감쇠 선형
	m_tInfo.fAttenuation2	= 0.0f; // 감쇠 2차
	m_tInfo.fFalloff		= 1.0f; // 스포트라이트 감쇠
	m_tInfo.fRange			= 100.0f; // 빛의 범위
	m_tInfo.fTheta			= XM_PI / 4; // 스포트라이트 내부 각도 (45도)
	m_tInfo.fPhi			= XM_PI / 2; // 스포트라이트 외부 각도 (90도)
	return S_OK;
}

HRESULT CLight::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		//LIGHT_DESC in = *reinterpret_cast<LIGHT_DESC*>(pArg);
		//memcpy(&m_tInfo, &in, sizeof(LIGHT_DESC));
		LIGHT_INIT file = *reinterpret_cast<LIGHT_INIT*>(pArg);
		simdjson::dom::parser parser;;
		simdjson::dom::element data = parser.load(WideToUtf8(file.strLightPath.c_str()));
		std::string_view type_sv = data["Type"].get_string();
		if (type_sv == "Point") m_tInfo.eType = LT_POINT;
		else if (type_sv == "Spot") m_tInfo.eType = LT_SPOT;
		else if (type_sv == "Direction") m_tInfo.eType = LT_DIR;
		m_tInfo.fAmbient = { (float)data["Ambient"]["x"].get_double(), (float)data["Ambient"]["y"].get_double(), (float)data["Ambient"]["z"].get_double(), (float)data["Ambient"]["w"].get_double() };
		m_tInfo.fSpecular = { (float)data["Specular"]["x"].get_double(), (float)data["Specular"]["y"].get_double(), (float)data["Specular"]["z"].get_double(), (float)data["Specular"]["w"].get_double() };
		m_tInfo.fDirection = { (float)data["Direction"]["x"].get_double(), (float)data["Direction"]["y"].get_double(), (float)data["Direction"]["z"].get_double()};
		m_tInfo.fAttenuation0 = { (float)data["Attenuation0"].get_double()};
		m_tInfo.fAttenuation1 = { (float)data["Attenuation1"].get_double() };
		m_tInfo.fAttenuation2 = { (float)data["Attenuation2"].get_double() };
		m_tInfo.fFalloff = { (float)data["Falloff"].get_double() };
		m_tInfo.fRange = { (float)data["Range"].get_double() };
		m_tInfo.fTheta = { XM_PI / (float)data["Theta"].get_double() };
		m_tInfo.fPhi = { XM_PI / (float)data["Phi"].get_double() };
	}
	return S_OK;
}

HRESULT CLight::Bind_Resouce(_uint iIndex)
{
	/*D3DLIGHT9 light = {};
	light.Type = (D3DLIGHTTYPE)m_tInfo.eType;
	memcpy(&light.Diffuse,		&m_tInfo.fDiffuse,		sizeof(_float4));
	memcpy(&light.Ambient,		&m_tInfo.fAmbient,		sizeof(_float4));
	memcpy(&light.Specular,		&m_tInfo.fSpecular,		sizeof(_float4));
	memcpy(&light.Position,		&m_tInfo.fPosition,		sizeof(_float3));
	memcpy(&light.Direction,	&m_tInfo.fDirection,	sizeof(_float3));
	memcpy(&light.Attenuation0, &m_tInfo.fAttenuation0,	sizeof(_float));
	memcpy(&light.Attenuation1, &m_tInfo.fAttenuation1,	sizeof(_float));
	memcpy(&light.Attenuation2, &m_tInfo.fAttenuation2,	sizeof(_float));
	memcpy(&light.Falloff,		&m_tInfo.fFalloff,		sizeof(_float));
	memcpy(&light.Range,		&m_tInfo.fRange,		sizeof(_float));
	memcpy(&light.Theta,		&m_tInfo.fTheta,		sizeof(_float));
	memcpy(&light.Phi,			&m_tInfo.fPhi,			sizeof(_float));

	m_pDevice->SetLight(iIndex, &light);*/

	return S_OK;
}

HRESULT CLight::Bind_Light(CShader* pShader)
{

	return S_OK;
}

CLight::SHADER_LIGHT CLight::Get_Shader_Light()
{
	SHADER_LIGHT out = {};
	out.eType			= static_cast<_int>(m_tInfo.eType);
	out.fDiffuse		= m_tInfo.fDiffuse;
	out.fPosition		= _float4(m_tInfo.fPosition.x, m_tInfo.fPosition.y, m_tInfo.fPosition.z, 1.0f);
	out.fDirection		= _float4(m_tInfo.fDirection.x, m_tInfo.fDirection.y, m_tInfo.fDirection.z, 1.0f);
	out.fRange			= m_tInfo.fRange;
	out.fAttenuation0	= m_tInfo.fAttenuation0;
	out.fAttenuation1	= m_tInfo.fAttenuation1;
	out.fAttenuation2	= m_tInfo.fAttenuation2;
	out.fFallOff		= m_tInfo.fFalloff;
	out.fTheta			= m_tInfo.fTheta;
	out.fPhi			= m_tInfo.fPhi;
	return out;
}

void CLight::DecreaseIntensity(_uint iFrame)
{
	//0123
	m_tInfo.fAttenuation1 = iFrame / 5.f;
	//m_tInfo.fAttenuation2 = iFrame;
}

CLight* CLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LIGHTTYPE eType)
{
	CLight* pInstance = new CLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX("Failed to Created : CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CLight* CLight::Clone(void* pArg)
{
	CLight* pInstance = new CLight(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	__super::Free();
}
