#include "Light.h"

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
	}
	return S_OK;
}

void CLight::Free()
{
	__super::Free();
}
#pragma endregion


HRESULT CLight::RenderImGui()
{
	if(ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
	{

	}
	return S_OK;
}

