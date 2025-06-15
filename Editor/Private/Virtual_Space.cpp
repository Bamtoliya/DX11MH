
#include "Virtual_Space.h"
#include "GameInstance.h"


#pragma region Constructor/Destructor
CVirtual_Space::CVirtual_Space(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CVirtual_Space::CVirtual_Space(const CVirtual_Space& Prototype)
	: CGameObject{ Prototype }
{

}
CVirtual_Space* CVirtual_Space::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVirtual_Space* pInstance = new CVirtual_Space(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVirtual_Space");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CVirtual_Space::Clone(void* pArg)
{
	CVirtual_Space* pInstance = new CVirtual_Space(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVirtual_Space");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVirtual_Space::Clone(simdjson::ondemand::object& jsonData)
{
	CVirtual_Space* pInstance = new CVirtual_Space(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Created : CVirtual_Space");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CVirtual_Space::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVirtual_Space::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};
	if (pArg != nullptr)
		memcpy(&Desc, pArg, sizeof(GAMEOBJECT_DESC));
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	if (nullptr == pArg)
	{
		lstrcpy(Desc.szName, TEXT("Virtual_Space"));
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

HRESULT CVirtual_Space::Initialize(simdjson::ondemand::object& jsonData)
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

HRESULT CVirtual_Space::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Model_Virtual_Space"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
 		return E_FAIL;

	CRigidBody::RIGIDBODY_DESC rigidBodyDesc = {};
	rigidBodyDesc.pModelForMesh = m_pModelCom;
	rigidBodyDesc.bIsConvex = true;
	for (auto pMesh : m_pModelCom->Get_Meshes())
	{
		CRigidBody::SHAPE_DESC shapeDesc;
		shapeDesc.eShapeType = CRigidBody::EShapeType::TRIANGLE_MESH;
		shapeDesc.pMesh = pMesh;
		rigidBodyDesc.vShapeDescs.push_back(shapeDesc);
	}

	/* For.Com_HeightField */
	/*if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_HeightField"),
		TEXT("Com_HeightField"), reinterpret_cast<CComponent**>(&m_pHeightFieldCom))))
		return E_FAIL;*/
	
	///* For.Com_RigidBody */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &rigidBodyDesc)))
		return E_FAIL;

	return S_OK;
}

void CVirtual_Space::Free()
{
	__super::Free();
	//Safe_Release(m_pHeightFieldCom);
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}

#pragma endregion

#pragma region Save&Load

HRESULT CVirtual_Space::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CVirtual_Space"] = data;
	outJson["CVirtual_Space"]["PrototypeTag"] = "Prototype_GameObject_Virtual_Space";
	return S_OK;
}

HRESULT CVirtual_Space::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

HRESULT CVirtual_Space::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CVirtual_Space";
	return S_OK;
}

HRESULT CVirtual_Space::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma endregion

#pragma region Loops
void CVirtual_Space::Priority_Update(_float fTimeDelta)
{

}

void CVirtual_Space::Update(_float fTimeDelta)
{

}

void CVirtual_Space::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CVirtual_Space::Bind_ShaderResources()
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


HRESULT CVirtual_Space::Render()
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