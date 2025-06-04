#include "Component.h"
#include "GameObject.h"
#include "GameInstance.h"
CComponent::CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_bIsCloned { false } 
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CComponent::CComponent(const CComponent& Prototype)
	: m_pDevice{ Prototype.m_pDevice }
	, m_pContext{ Prototype.m_pContext }
	, m_pGameInstance{ Prototype.m_pGameInstance }
	, m_bIsCloned{ true }
	, m_bActive{ Prototype.m_bActive }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CComponent::Initialize(void* pArg)
{
	return S_OK;
}

CComponent* CComponent::Clone(CGameObject* pGameObject, void* pArg)
{
	CComponent* pInstance = Clone(pArg);
	if (nullptr == pInstance)
	{
		MSG_BOX("Failed to Cloned with Owner : CComponent");
		Safe_Release(pInstance);
	}
	else
	{
		pInstance->m_pOwner = pGameObject;
		if (FAILED(pInstance->Late_Initialize()))
		{
			MSG_BOX("Failed to Late Initialize : CComponent");
			Safe_Release(pInstance);
		}
	}
	
	return pInstance;
}
void CComponent::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	__super::Free();
}

void CComponent::Set_OwnerGameObject(CGameObject* pGameObject)
{
	m_pOwner = pGameObject;
}


HRESULT CComponent::Serialize(json& data)
{
	data["Active"] = m_bActive;
	return S_OK;
}


HRESULT CComponent::Deserialize(simdjson::ondemand::object& jsonData)
{
	jsonData["Active"].get_bool().get(m_bActive);
	return S_OK;
}