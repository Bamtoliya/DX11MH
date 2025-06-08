#include "Terrain.h"

#include "GameInstance.h"

#pragma region Constructor/Destructor
CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CTerrain::CTerrain(const CTerrain& Prototype)
	: CGameObject{ Prototype }
{

}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain::Clone(simdjson::ondemand::object& jsonData)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Cloned : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CTerrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};

	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	if(pArg != nullptr)
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

HRESULT CTerrain::Initialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	if(FAILED(Deserialize(jsonData)))
		return E_FAIL;
	if (FAILED(Late_Initialize()))
		return E_FAIL;
	return S_OK;
}

HRESULT CTerrain::Ready_Components()
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

	/* For.Com_Splat_Texture */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Texture_Terrain_SplatMap"),
		TEXT("Com_Splat_Texture"), reinterpret_cast<CComponent**>(&m_pSplatMapTextureCom))))
		return E_FAIL;

	/* For.Com_Splat_Texture */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Texture_Terrain_Water"),
		TEXT("Com_Water_Texture"), reinterpret_cast<CComponent**>(&m_pWaterTextureCom))))
		return E_FAIL;


	/* For.Com_Texture_BaseDiffuse */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Texture_Terrain_BaseDiffuse"),
		TEXT("Com_Texture_BaseDiffuse"), reinterpret_cast<CComponent**>(&m_pBaseDiffuseCom))))
		return E_FAIL;

	/* For.Com_Texture_BaseParameter */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Texture_Terrain_BaseParameter"),
		TEXT("Com_Texture_BaseParameter"), reinterpret_cast<CComponent**>(&m_pBaseParameterCom))))
		return E_FAIL;


	/* For.Com_Texture_DetailDiffuse */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Texture_Terrain_DetailDiffuse"),
		TEXT("Com_Texture_DetailDiffuse"), reinterpret_cast<CComponent**>(&m_pDetailDiffuseCom))))
		return E_FAIL;

	/* For.Com_Texture_DetailHeight */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Texture_Terrain_DetailHeight"),
		TEXT("Com_Texture_DetailHeight"), reinterpret_cast<CComponent**>(&m_pDetailHeightCom))))
		return E_FAIL;

	/* For.Com_Texture_DetailNormal */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Texture_Terrain_DetailNormal"),
		TEXT("Com_Texture_DetailNormal"), reinterpret_cast<CComponent**>(&m_pDetailNormalCom))))
		return E_FAIL;

	/* For.Com_HeightField */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_HeightField"),
		TEXT("Com_HeightField"), reinterpret_cast<CComponent**>(&m_pHeightFieldCom))))
		return E_FAIL;

	return S_OK;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pHeightFieldCom);

	Safe_Release(m_pSplatMapTextureCom);
	Safe_Release(m_pWaterTextureCom);
	Safe_Release(m_pBaseDiffuseCom);
	Safe_Release(m_pBaseParameterCom);
	Safe_Release(m_pDetailDiffuseCom);
	Safe_Release(m_pDetailHeightCom);
	Safe_Release(m_pDetailNormalCom);
}
#pragma endregion

HRESULT CTerrain::Serialize(json& outJson)
{
	json data;
	if(FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CTerrain"] = data;
	outJson["CTerrain"]["PrototypeTag"] = "Prototype_GameObject_Terrain";
	return S_OK;
}

HRESULT CTerrain::Deserialize(simdjson::ondemand::object & jsonData)
{
	if(FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;	
	return S_OK;
}

HRESULT CTerrain::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CTerrain";
	return S_OK;
}

HRESULT CTerrain::Prototype_Deserialize(simdjson::ondemand::object & jsonData)
{
	return S_OK;
}

#pragma region Loops
void CTerrain::Priority_Update(_float fTimeDelta)
{

}

void CTerrain::Update(_float fTimeDelta)
{

}
void CTerrain::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CTerrain::Bind_ShaderResources()
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

	if (FAILED(m_pWaterTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pSplatMapTextureCom->Bind_ShaderResource(m_pShaderCom, "g_SplatMapTexture", 0)))
		return E_FAIL;

#pragma region TextureArray
	if (FAILED(m_pBaseDiffuseCom->Bind_ShaderResource(m_pShaderCom, "g_BaseDiffuseMaps", 0)))
		return E_FAIL;

	if (FAILED(m_pBaseDiffuseCom->Bind_IndexMap(m_pShaderCom, "g_BaseDiffuseIndexMap")))
		return E_FAIL;

	if (FAILED(m_pBaseParameterCom->Bind_ShaderResource(m_pShaderCom, "g_BaseParameterMaps", 0)))
		return E_FAIL;

	if (FAILED(m_pBaseParameterCom->Bind_IndexMap(m_pShaderCom, "g_BaseParameterIndexMap")))
		return E_FAIL;

	if (FAILED(m_pDetailDiffuseCom->Bind_ShaderResource(m_pShaderCom, "g_DetailDiffuseMaps", 0)))
		return E_FAIL;

	if (FAILED(m_pDetailDiffuseCom->Bind_IndexMap(m_pShaderCom, "g_DetailDiffuseIndexMap")))
		return E_FAIL;

	if (FAILED(m_pDetailHeightCom->Bind_ShaderResource(m_pShaderCom, "g_DetailHeightMaps", 0)))
		return E_FAIL;

	if (FAILED(m_pDetailHeightCom->Bind_IndexMap(m_pShaderCom, "g_DetailHeightIndexMap")))
		return E_FAIL;

	if (FAILED(m_pDetailNormalCom->Bind_ShaderResource(m_pShaderCom, "g_DetailNormalMaps", 0)))
		return E_FAIL;

	if (FAILED(m_pDetailNormalCom->Bind_IndexMap(m_pShaderCom, "g_DetailNormalIndexMap")))
		return E_FAIL;
#pragma endregion

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

HRESULT CTerrain::Render()
{
	if (!m_bVisible)
		return S_OK;
	if(FAILED(Bind_ShaderResources()))
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