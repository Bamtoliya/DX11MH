#include "GameObject_Open.h"
#include "GameInstance.h"

#pragma region Constructor/Destructor
CGameObject_Open::CGameObject_Open(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CGameObject_Open::CGameObject_Open(const CGameObject_Open& Prototype)
	: CGameObject{ Prototype }
{

}
CGameObject_Open* CGameObject_Open::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameObject_Open* pInstance = new CGameObject_Open(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGameObject_Open");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CGameObject_Open::Clone(void* pArg)
{
	CGameObject_Open* pInstance = new CGameObject_Open(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGameObject_Open");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGameObject_Open::Clone(simdjson::ondemand::object& jsonData)
{
	CGameObject_Open* pInstance = new CGameObject_Open(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Cloned : CGameObject_Open");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CGameObject_Open::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject_Open::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};
	if (pArg != nullptr)
		memcpy(&Desc, pArg, sizeof(GAMEOBJECT_DESC));
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	if (nullptr == pArg)
	{
		lstrcpy(Desc.szName, TEXT("GameObject_Open"));
	}
	else
	{
		lstrcpy(Desc.szName, static_cast<GAMEOBJECT_DESC*>(pArg)->szName);
	}

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Late_Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject_Open::Initialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Deserialize(jsonData)))
		return E_FAIL;

	if (FAILED(Late_Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject_Open::Ready_Components()
{
	return S_OK;
}

void CGameObject_Open::Free()
{
	__super::Free();
	Safe_Release(m_pModelColliderCom);
	Safe_Release(m_pRigidBodyCom);
}

#pragma endregion

#pragma region Save&Load

HRESULT CGameObject_Open::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CGameObject_Open"] = data;
	outJson["CGameObject_Open"]["PrototypeTag"] = "Prototype_GameObject_Open";
	return S_OK;
}

HRESULT CGameObject_Open::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

HRESULT CGameObject_Open::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CGameObject_Open";
	return S_OK;
}

HRESULT CGameObject_Open::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma endregion

#pragma region Loops
void CGameObject_Open::Priority_Update(_float fTimeDelta)
{

}

void CGameObject_Open::Update(_float fTimeDelta)
{

}

void CGameObject_Open::Late_Update(_float fTimeDelta)
{
	//m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CGameObject_Open::Render()
{
	if (!m_bVisible)
		return S_OK;

	return S_OK;
}
#pragma endregion