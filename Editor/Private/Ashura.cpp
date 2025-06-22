#include "Ashura.h"
#include "GameInstance.h"
#include "Ashura_States.h"

#pragma region Constructor/Destructor
CAshura::CAshura(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CAshura::CAshura(const CAshura& Prototype)
	: CGameObject{ Prototype }
{

}
CAshura* CAshura::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAshura* pInstance = new CAshura(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAshura");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CAshura::Clone(void* pArg)
{
	CAshura* pInstance = new CAshura(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAshura");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAshura::Clone(simdjson::ondemand::object& jsonData)
{
	CAshura* pInstance = new CAshura(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Created : CAshura");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CAshura::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAshura::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};
	if (pArg != nullptr)
		memcpy(&Desc, pArg, sizeof(GAMEOBJECT_DESC));
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	if (nullptr == pArg)
	{
		lstrcpy(Desc.szName, TEXT("Ashura"));
	}
	else
	{
		lstrcpy(Desc.szName, static_cast<GAMEOBJECT_DESC*>(pArg)->szName);
	}

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	if (FAILED(Late_Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAshura::Initialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Deserialize(jsonData)))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	if (FAILED(Late_Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAshura::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Model_Ashura"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Model_Ashura_Collider"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelColliderCom))))
		return E_FAIL;

	/* For.Com_Animator */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom))))
		return E_FAIL;

	/* For.Com_Controller */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Ashura_Controller"),
		TEXT("Com_Controller"), reinterpret_cast<CComponent**>(&m_pControllerCom))))
		return E_FAIL;

	
	CRigidBody::RIGIDBODY_DESC rigidBodyDesc = {};
	rigidBodyDesc.pModelForMesh = m_pModelColliderCom;
	rigidBodyDesc.bIsConvex = false;
	for (auto pMesh : m_pModelColliderCom->Get_Meshes())
	{
		CRigidBody::SHAPE_DESC shapeDesc;
		shapeDesc.eShapeType = CRigidBody::EShapeType::TRIANGLE_MESH;
		shapeDesc.pMesh = pMesh;
		shapeDesc.bIsDoubleSided = false;
		shapeDesc.bIsQueryShape = false;
		rigidBodyDesc.vShapeDescs.push_back(shapeDesc);
	}

	/* For.Com_RigidBody */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &rigidBodyDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAshura::Ready_States()
{
	m_pControllerCom->Add_State(CAshura_Movement_State::Create(this));
	//m_pControllerCom->Add_State(CSonic_Combat_State::Create(this));
	return S_OK;
}

void CAshura::Free()
{
	__super::Free();
	Safe_Release(m_pControllerCom);
	Safe_Release(m_pAnimatorCom);
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pModelColliderCom);
	Safe_Release(m_pShaderCom);
}

#pragma endregion

#pragma region Save&Load

HRESULT CAshura::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CAshura"] = data;
	outJson["CAshura"]["PrototypeTag"] = "Prototype_GameObject_Ashura";
	return S_OK;
}

HRESULT CAshura::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

HRESULT CAshura::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CAshura";
	return S_OK;
}

HRESULT CAshura::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma endregion

#pragma region Loops
void CAshura::Priority_Update(_float fTimeDelta)
{
	if (m_pRigidBodyCom)
		m_pRigidBodyCom->Priority_Update(fTimeDelta);
}

void CAshura::Update(_float fTimeDelta)
{
	if (m_pControllerCom)
		m_pControllerCom->Update(fTimeDelta);
	if (m_pAnimatorCom)
		m_pAnimatorCom->Update(fTimeDelta);
}

void CAshura::Late_Update(_float fTimeDelta)
{
	if (m_pRigidBodyCom)
		m_pRigidBodyCom->Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CAshura::Bind_ShaderResources()
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


HRESULT CAshura::Render()
{
	if (!m_bVisible)
		return S_OK;
	Bind_ShaderResources();

	_uint		iNumMesh = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMesh; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		m_pModelCom->Bind_Bone_Matrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}
#pragma endregion