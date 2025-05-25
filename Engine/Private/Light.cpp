#include "Light.h"
#include "GameObject.h"

#pragma region Constructor/ Destructor
CLight::CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CLight::CLight(const CLight& Prototype)
	: CComponent{ Prototype }
	, m_tInfo{ Prototype.m_tInfo }
{
}

CLight* CLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLight* pInstance = new CLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
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
		MSG_BOX("Failed to Cloned : CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CLight::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLight::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		LIGHT_DESC* pDesc = static_cast<LIGHT_DESC*>(pArg);
		memcpy(&m_tInfo, pDesc, sizeof(LIGHT_DESC));
	}
	return S_OK;
}

void CLight::Free()
{
	__super::Free();
}
#pragma endregion


#pragma region Save&Load

HRESULT CLight::Serialize(json& outJson)
{
	return S_OK;
}

HRESULT CLight::Deserialize(const json& inData)
{
	return S_OK;
}

HRESULT CLight::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CLight";
	return S_OK;
}

HRESULT CLight::Prototype_Deserialize(const simdjson::ondemand::object& jsonData)
{

	return S_OK;
}

#pragma endregion

void CLight::Update()
{
	if (!m_pOwner)
		return;

	XMStoreFloat4(&m_tInfo.vPosition, m_pOwner->Get_Position());
}

HRESULT CLight::RenderImGui()
{
	if(ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Combo("Light Type", reinterpret_cast<int*>(&m_tInfo.eType), "Directional\0Point\0\0");

		if (m_tInfo.eType == LIGHT_DESC::TYPE_DIRECTIONAL)
			ImGui::DragFloat3("Direction", &m_tInfo.vDirection.x, 0.1f, -1.f, 1.f);
		if (m_tInfo.eType == LIGHT_DESC::TYPE_POINT)
			ImGui::DragFloat("Range", &m_tInfo.fRange, 1.0f, 0.f, 1000.f);
		ImGui::ColorPicker4("Diffuse", &m_tInfo.vDiffuse.x);
		ImGui::ColorPicker4("Ambient", &m_tInfo.vAmbient.x);
		ImGui::ColorPicker4("Specular", &m_tInfo.vSpecular.x);
	}
	return S_OK;
}

