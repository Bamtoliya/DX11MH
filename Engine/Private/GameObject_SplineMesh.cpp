
#include "GameObject_SplineMesh.h"
#include "GameInstance.h"

#pragma region Constructor/Destructor
CGameObject_SplineMesh::CGameObject_SplineMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CGameObject_SplineMesh::CGameObject_SplineMesh(const CGameObject_SplineMesh& Prototype)
	: CGameObject{ Prototype }
{

}
CGameObject_SplineMesh* CGameObject_SplineMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameObject_SplineMesh* pInstance = new CGameObject_SplineMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGameObject_SplineMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CGameObject_SplineMesh::Clone(void* pArg)
{
	CGameObject_SplineMesh* pInstance = new CGameObject_SplineMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGameObject_SplineMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGameObject_SplineMesh::Clone(simdjson::ondemand::object& jsonData)
{
	CGameObject_SplineMesh* pInstance = new CGameObject_SplineMesh(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Created : CGameObject_SplineMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CGameObject_SplineMesh::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject_SplineMesh::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};
	if (pArg != nullptr)
		memcpy(&Desc, pArg, sizeof(GAMEOBJECT_DESC));
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	if (nullptr == pArg)
	{
		lstrcpy(Desc.szName, TEXT("GameObject_SplineMesh"));
	}
	else
	{
		lstrcpy(Desc.szName, static_cast<GAMEOBJECT_DESC*>(pArg)->szName);
	}

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject_SplineMesh::Initialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Deserialize(jsonData)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject_SplineMesh::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_Shader_VtxMeshInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Model_GrindRail"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CVIBuffer_Mesh_Instance::MESH_INSTANCE_DESC meshInstanceDesc;
	meshInstanceDesc.pMesh = m_pModelCom->Get_Meshes()[0];

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_VIBuffer_Mesh_Instance"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferInstanceCom), &meshInstanceDesc)))
		return E_FAIL;

	/* For.Com_Spline */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Spline"),
		TEXT("Com_Spline"), reinterpret_cast<CComponent**>(&m_pSplineCom))))
		return E_FAIL;


	return S_OK;
}

void CGameObject_SplineMesh::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferInstanceCom);
	Safe_Release(m_pSplineCom);
}

#pragma endregion

#pragma region Save&Load

HRESULT CGameObject_SplineMesh::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CGameObject_SplineMesh"] = data;
	outJson["CGameObject_SplineMesh"]["PrototypeTag"] = "Prototype_GameObject_GameObject_SplineMesh";
	return S_OK;
}

HRESULT CGameObject_SplineMesh::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

HRESULT CGameObject_SplineMesh::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CGameObject_SplineMesh";
	return S_OK;
}

HRESULT CGameObject_SplineMesh::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma endregion

#pragma region Loops
void CGameObject_SplineMesh::Priority_Update(_float fTimeDelta)
{

}

void CGameObject_SplineMesh::Update(_float fTimeDelta)
{

}

void CGameObject_SplineMesh::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CGameObject_SplineMesh::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("objectId", &m_oID, sizeof(_uint))))
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


HRESULT CGameObject_SplineMesh::Render()
{
	if (!m_bVisible)
		return S_OK;
	Bind_ShaderResources();

	_uint		iNumMesh = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMesh; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		//m_pModelCom->Bind_Bone_Matrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}
#pragma endregion