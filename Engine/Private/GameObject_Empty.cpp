
#include "GameObject_Empty.h"
#include "GameInstance.h"

#pragma region Constructor/Destructor
CGameObject_Empty::CGameObject_Empty(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CGameObject_Empty::CGameObject_Empty(const CGameObject_Empty& Prototype)
	: CGameObject{ Prototype }
{

}
CGameObject_Empty* CGameObject_Empty::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameObject_Empty* pInstance = new CGameObject_Empty(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGameObject_Empty");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CGameObject_Empty::Clone(void* pArg)
{
	CGameObject_Empty* pInstance = new CGameObject_Empty(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGameObject_Empty");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGameObject_Empty::Clone(simdjson::ondemand::object& jsonData)
{
	CGameObject_Empty* pInstance = new CGameObject_Empty(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Created : CGameObject_Empty");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CGameObject_Empty::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject_Empty::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};
	if (pArg != nullptr)
		memcpy(&Desc, pArg, sizeof(GAMEOBJECT_DESC));
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	if (nullptr == pArg)
	{
		lstrcpy(Desc.szName, TEXT("GameObject_Empty"));
	}
	else
	{
		lstrcpy(Desc.szName, static_cast<GAMEOBJECT_DESC*>(pArg)->szName);
	}

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject_Empty::Initialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if (FAILED(Deserialize(jsonData)))
		return E_FAIL;

	return S_OK;
}

void CGameObject_Empty::Free()
{
	__super::Free();
}

#pragma endregion

#pragma region Save&Load

HRESULT CGameObject_Empty::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CGameObject_Empty"] = data;
	outJson["CGameObject_Empty"]["PrototypeTag"] = "Prototype_GameObject_GameObject_Empty";
	return S_OK;
}

HRESULT CGameObject_Empty::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

HRESULT CGameObject_Empty::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CGameObject_Empty";
	return S_OK;
}

HRESULT CGameObject_Empty::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma endregion

#pragma region Loops
void CGameObject_Empty::Priority_Update(_float fTimeDelta)
{

}

void CGameObject_Empty::Update(_float fTimeDelta)
{

}

void CGameObject_Empty::Late_Update(_float fTimeDelta)
{
	//m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CGameObject_Empty::Pre_Render()
{
	return S_OK;
}

HRESULT CGameObject_Empty::Render()
{
	return S_OK;
}
HRESULT CGameObject_Empty::Post_Render()
{
	return S_OK;
}
#pragma endregion