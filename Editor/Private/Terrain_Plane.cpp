#include "Terrain_Plane.h"

#include "GameInstance.h"

#pragma region Constructor/Destructor
CTerrain_Plane::CTerrain_Plane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CTerrain_Plane::CTerrain_Plane(const CTerrain_Plane& Prototype)
	: CGameObject{ Prototype }
{

}

CTerrain_Plane* CTerrain_Plane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain_Plane* pInstance = new CTerrain_Plane(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrain_Plane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain_Plane::Clone(void* pArg)
{
	CTerrain_Plane* pInstance = new CTerrain_Plane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain_Plane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain_Plane::Clone(simdjson::ondemand::object& jsonData)
{
	CTerrain_Plane* pInstance = new CTerrain_Plane(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Cloned : CTerrain_Plane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CTerrain_Plane::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain_Plane::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};

	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	if (pArg != nullptr)
	{
		GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
		lstrcpy(Desc.szName, pDesc->szName);
	}
	else
	{
		lstrcpy(Desc.szName, TEXT("Terrain"));
	}

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain_Plane::Initialize(simdjson::ondemand::object& jsonData)
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

HRESULT CTerrain_Plane::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_Terrain"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_HeightField */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_HeightField"),
		TEXT("Com_HeightField"), reinterpret_cast<CComponent**>(&m_pHeightFieldCom))))
		return E_FAIL;

	return S_OK;
}

void CTerrain_Plane::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pHeightFieldCom);
}
#pragma endregion

HRESULT CTerrain_Plane::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CTerrain_Plane"] = data;
	outJson["CTerrain_Plane"]["PrototypeTag"] = "Prototype_GameObject_Terrain";
	return S_OK;
}

HRESULT CTerrain_Plane::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

HRESULT CTerrain_Plane::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CTerrain_Plane";
	return S_OK;
}

HRESULT CTerrain_Plane::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma region Loops
void CTerrain_Plane::Priority_Update(_float fTimeDelta)
{

}

void CTerrain_Plane::Update(_float fTimeDelta)
{

}
void CTerrain_Plane::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CTerrain_Plane::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	/* dx9 : 장치에 뷰, 투영행렬을 저장해두면 렌더링시 알아서 정점에 Transform해주었다. */
	/* dx11 : 셰이더에 뷰, 투영행렬을 저장해두고 우리가 직접 변환해주어야한다. */
	if (FAILED(m_pShaderCom->Bind_RawValue("objectId", &m_oID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_Light(0);
	if (pLightDesc == nullptr)
		return S_OK;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain_Plane::Render()
{
	if (!m_bVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	return S_OK;
}
#pragma endregion