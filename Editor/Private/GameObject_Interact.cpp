
#include "GameObject_Interact.h"
#include "GameInstance.h"

#pragma region Constructor/Destructor
CGameObject_Interact::CGameObject_Interact(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CGameObject_Interact::CGameObject_Interact(const CGameObject_Interact& Prototype)
	: CGameObject{ Prototype }
{

}
CGameObject_Interact* CGameObject_Interact::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameObject_Interact* pInstance = new CGameObject_Interact(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGameObject_Interact");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CGameObject_Interact::Clone(void* pArg)
{
	CGameObject_Interact* pInstance = new CGameObject_Interact(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGameObject_Interact");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGameObject_Interact::Clone(simdjson::ondemand::object& jsonData)
{
	CGameObject_Interact* pInstance = new CGameObject_Interact(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Created : CGameObject_Interact");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CGameObject_Interact::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject_Interact::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};
	if (pArg != nullptr)
		memcpy(&Desc, pArg, sizeof(GAMEOBJECT_DESC));
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	if (nullptr == pArg)
	{
		lstrcpy(Desc.szName, TEXT("GameObject_Interact"));
	}
	else
	{
		lstrcpy(Desc.szName, static_cast<GAMEOBJECT_DESC*>(pArg)->szName);
	}

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject_Interact::Initialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if (FAILED(Deserialize(jsonData)))
		return E_FAIL;

	return S_OK;
}

void CGameObject_Interact::Free()
{
	__super::Free();
}

#pragma endregion

#pragma region Save&Load

HRESULT CGameObject_Interact::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CGameObject_Interact"] = data;
	outJson["CGameObject_Interact"]["PrototypeTag"] = "Prototype_GameObject_GameObject_Interact";
	return S_OK;
}

HRESULT CGameObject_Interact::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

HRESULT CGameObject_Interact::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CGameObject_Interact";
	return S_OK;
}

HRESULT CGameObject_Interact::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma endregion

#pragma region Loops
void CGameObject_Interact::Priority_Update(_float fTimeDelta)
{

}

void CGameObject_Interact::Update(_float fTimeDelta)
{

}

void CGameObject_Interact::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CGameObject_Interact::Render()
{
	if (!m_bVisible)
		return S_OK;

	return S_OK;
}
void CGameObject_Interact::OnCollisionEnter(PxActor* pOther, const PxContactPair& cp)
{
}
void CGameObject_Interact::OnCollisionOverlapping(PxActor* pOther)
{
}
void CGameObject_Interact::OnCollisionExit(PxActor* pOther)
{
}
void CGameObject_Interact::LockOn()
{

}
#pragma endregion