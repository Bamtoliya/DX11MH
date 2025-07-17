#pragma once
#include "DashPanel.h"
#include "GameInstance.h"
#include "Sonic_Controller.h"
#include "Material.h"

#pragma region Constructor/Destructor
CDashPanel::CDashPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CDashPanel::CDashPanel(const CDashPanel& Prototype)
	: CGameObject{ Prototype }
{

}
CDashPanel* CDashPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDashPanel* pInstance = new CDashPanel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDashPanel");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CDashPanel::Clone(void* pArg)
{
	CDashPanel* pInstance = new CDashPanel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDashPanel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDashPanel::Clone(simdjson::ondemand::object& jsonData)
{
	CDashPanel* pInstance = new CDashPanel(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Cloned : CDashPanel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CDashPanel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDashPanel::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};
	if (pArg != nullptr)
		memcpy(&Desc, pArg, sizeof(GAMEOBJECT_DESC));
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	if (nullptr == pArg)
	{
		lstrcpy(Desc.szName, TEXT("DashPanel"));
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

HRESULT CDashPanel::Initialize(simdjson::ondemand::object& jsonData)
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

HRESULT CDashPanel::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Model_DashPanel"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CRigidBody::RIGIDBODY_DESC rigidBodyDesc = {};
	rigidBodyDesc.vLocalPosition = { 0.f, 0.5f, 0.f };
	CRigidBody::SHAPE_DESC shapeDesc = {};
	shapeDesc.eShapeType = CRigidBody::EShapeType::SPHERE;
	shapeDesc.vShapeSize = { 0.75f, 1.0, 1.f };
	rigidBodyDesc.vShapeDescs.push_back(shapeDesc);
	rigidBodyDesc.eBodyType = CRigidBody::EBodyType::STATIC;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &rigidBodyDesc)))
		return E_FAIL;


	return S_OK;
}

void CDashPanel::Free()
{
	__super::Free();
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}

#pragma endregion

#pragma region Save&Load

HRESULT CDashPanel::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CDashPanel"] = data;
	outJson["CDashPanel"]["PrototypeTag"] = "Prototype_GameObject_DashPanel";
	return S_OK;
}

HRESULT CDashPanel::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

HRESULT CDashPanel::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CDashPanel";
	return S_OK;
}

HRESULT CDashPanel::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma endregion

#pragma region Loops
void CDashPanel::Priority_Update(_float fTimeDelta)
{

}

void CDashPanel::Update(_float fTimeDelta)
{
	if (m_pRigidBodyCom)
		m_pRigidBodyCom->Update(fTimeDelta);
}

void CDashPanel::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CDashPanel::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("objectId", &m_oID, sizeof(_uint))))
		return E_FAIL;

	return S_OK;
}


HRESULT CDashPanel::Render()
{
	if (!m_bVisible)
		return S_OK;
	Bind_ShaderResources();

	_uint		iNumMesh = m_pModelCom->Get_NumMeshes();

	bool isTexture = false;
	_uint iMaterialIndex = 0;
	for (_uint i = 0; i < iNumMesh; i++)
	{
		iMaterialIndex = m_pModelCom->Get_Meshes()[i]->Get_MaterialIndex();
		isTexture = m_pModelCom->Get_Material(iMaterialIndex)->Has_Texture(aiTextureType_DIFFUSE);
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bDiffuseTexture", &isTexture, sizeof(_bool))))
			return E_FAIL;
		if (isTexture)
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
				return E_FAIL;

		isTexture = m_pModelCom->Get_Material(iMaterialIndex)->Has_Texture(aiTextureType_NORMALS);
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bNormalTexture", &isTexture, sizeof(_bool))))
			return E_FAIL;
		if (isTexture)
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0)))
				return E_FAIL;

		isTexture = m_pModelCom->Get_Material(iMaterialIndex)->Has_Texture(aiTextureType_SPECULAR);
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bParameterTexture", &isTexture, sizeof(_bool))))
			return E_FAIL;
		if (isTexture)
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ParameterTexture", i, aiTextureType_SPECULAR, 0)))
				return E_FAIL;

		isTexture = m_pModelCom->Get_Material(iMaterialIndex)->Has_Texture(aiTextureType_EMISSIVE);
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bEmissiveTexture", &isTexture, sizeof(_bool))))
			return E_FAIL;
		if (isTexture)
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE, 0)))
				return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}
void CDashPanel::OnCollisionEnter(PxActor* pOther, const PxContactPair& cp)
{
	if (m_pGameInstance->Get_CurrentPlayer() == pOther->userData)
	{
		CSonic_Controller* pController = dynamic_cast<CSonic_Controller*>(static_cast<CGameObject*>(pOther->userData)->Get_Component(L"Com_Controller"));
		if (pController != nullptr)
		{
			_vector vPushDirection = -m_pTransformCom->Get_State(STATE::LOOK);
			pController->DashPanel(vPushDirection);
		}

	}
}
void CDashPanel::OnCollisionOverlapping(PxActor* pOther)
{
}
void CDashPanel::OnCollisionExit(PxActor* pOther)
{
}
#pragma endregion