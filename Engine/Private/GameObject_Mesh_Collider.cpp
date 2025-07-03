
#include "GameObject_Mesh_Collider.h"
#include "GameInstance.h"

#pragma region Constructor/Destructor
CGameObject_Mesh_Collider::CGameObject_Mesh_Collider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CGameObject_Mesh_Collider::CGameObject_Mesh_Collider(const CGameObject_Mesh_Collider& Prototype)
	: CGameObject{ Prototype }
{

}
CGameObject_Mesh_Collider* CGameObject_Mesh_Collider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameObject_Mesh_Collider* pInstance = new CGameObject_Mesh_Collider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGameObject_Mesh_Collider");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CGameObject_Mesh_Collider::Clone(void* pArg)
{
	CGameObject_Mesh_Collider* pInstance = new CGameObject_Mesh_Collider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGameObject_Mesh_Collider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGameObject_Mesh_Collider::Clone(simdjson::ondemand::object& jsonData)
{
	CGameObject_Mesh_Collider* pInstance = new CGameObject_Mesh_Collider(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Created : CGameObject_Mesh_Collider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CGameObject_Mesh_Collider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject_Mesh_Collider::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};
	if (pArg != nullptr)
		memcpy(&Desc, pArg, sizeof(GAMEOBJECT_DESC));
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	if (nullptr == pArg)
	{
		lstrcpy(Desc.szName, TEXT("GameObject_Mesh_Collider"));
	}
	else
	{
		lstrcpy(Desc.szName, static_cast<GAMEOBJECT_DESC*>(pArg)->szName);
	}

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject_Mesh_Collider::Initialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if (FAILED(Deserialize(jsonData)))
		return E_FAIL;

	return S_OK;
}

void CGameObject_Mesh_Collider::Free()
{
	__super::Free();
}

#pragma endregion

#pragma region Save&Load

HRESULT CGameObject_Mesh_Collider::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CGameObject_Mesh_Collider"] = data;
	outJson["CGameObject_Mesh_Collider"]["PrototypeTag"] = "Prototype_GameObject_GameObject_Mesh_Collider";
	return S_OK;
}

HRESULT CGameObject_Mesh_Collider::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

HRESULT CGameObject_Mesh_Collider::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CGameObject_Mesh_Collider";
	return S_OK;
}

HRESULT CGameObject_Mesh_Collider::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma endregion

#pragma region Loops
void CGameObject_Mesh_Collider::Priority_Update(_float fTimeDelta)
{

}

void CGameObject_Mesh_Collider::Update(_float fTimeDelta)
{

}

void CGameObject_Mesh_Collider::Late_Update(_float fTimeDelta)
{
	//m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CGameObject_Mesh_Collider::Pre_Render()
{
	return S_OK;
}

HRESULT CGameObject_Mesh_Collider::Render()
{
	return S_OK;
}
HRESULT CGameObject_Mesh_Collider::Post_Render()
{
	return S_OK;
}
#pragma endregion