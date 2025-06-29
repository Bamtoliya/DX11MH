#include "GameObject_W6D01.h"
#include "GameInstance.h"

#pragma region Constructor/Destructor
CGameObject_W6D01::CGameObject_W6D01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CGameObject_W6D01::CGameObject_W6D01(const CGameObject_W6D01& Prototype)
	: CGameObject{ Prototype }
{

}
CGameObject_W6D01* CGameObject_W6D01::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameObject_W6D01* pInstance = new CGameObject_W6D01(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGameObject_W6D01");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CGameObject_W6D01::Clone(void* pArg)
{
	CGameObject_W6D01* pInstance = new CGameObject_W6D01(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGameObject_W6D01");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGameObject_W6D01::Clone(simdjson::ondemand::object& jsonData)
{
	CGameObject_W6D01* pInstance = new CGameObject_W6D01(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Cloned : CGameObject_W6D01");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CGameObject_W6D01::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject_W6D01::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};
	if (pArg != nullptr)
		memcpy(&Desc, pArg, sizeof(GAMEOBJECT_DESC));
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	if (nullptr == pArg)
	{
		lstrcpy(Desc.szName, TEXT("GameObject_W6D01"));
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

HRESULT CGameObject_W6D01::Initialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Deserialize(jsonData)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject_W6D01::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Model_W06D01"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Model_Collider */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Model_W06D01_Collider"),
		TEXT("Com_Model_Collider"), reinterpret_cast<CComponent**>(&m_pModelColliderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_VIBuffer_Mesh_Instance"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	m_pModelColliderCom;
	CRigidBody::RIGIDBODY_DESC rigidDesc;
	rigidDesc.eBodyType = CRigidBody::EBodyType::STATIC;
	rigidDesc.bIsConvex = false;
	_uint i = 0;
	for (auto& pMesh : m_pModelColliderCom->Get_Meshes())
	{
		CRigidBody::SHAPE_DESC shapeDesc;
		shapeDesc.pMesh = pMesh;
		shapeDesc.bIsQueryShape = true;
		shapeDesc.iMeshIndex = i++;
		shapeDesc.eShapeType = CRigidBody::EShapeType::TRIANGLE_MESH;
	}

	/* For.Com_RigidBody */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom))))
		return E_FAIL;


	return S_OK;
}

void CGameObject_W6D01::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pModelColliderCom);
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}

#pragma endregion

#pragma region Save&Load

HRESULT CGameObject_W6D01::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CGameObject_W6D01"] = data;
	outJson["CGameObject_W6D01"]["PrototypeTag"] = "Prototype_GameObject_GameObject_W6D01";
	return S_OK;
}

HRESULT CGameObject_W6D01::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

HRESULT CGameObject_W6D01::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CGameObject_W6D01";
	return S_OK;
}

HRESULT CGameObject_W6D01::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma endregion

#pragma region Loops
void CGameObject_W6D01::Priority_Update(_float fTimeDelta)
{

}

void CGameObject_W6D01::Update(_float fTimeDelta)
{

}

void CGameObject_W6D01::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CGameObject_W6D01::Bind_ShaderResources()
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


HRESULT CGameObject_W6D01::Render()
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