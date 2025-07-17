#include "DashRing.h"
#include "GameInstance.h"
#include "Sonic_Controller.h"
#include "Material.h"

#pragma region Constructor/Destructor
CDashRing::CDashRing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CDashRing::CDashRing(const CDashRing& Prototype)
	: CGameObject{ Prototype }
{

}
CDashRing* CDashRing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDashRing* pInstance = new CDashRing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDashRing");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CDashRing::Clone(void* pArg)
{
	CDashRing* pInstance = new CDashRing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDashRing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDashRing::Clone(simdjson::ondemand::object& jsonData)
{
	CDashRing* pInstance = new CDashRing(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Cloned : CDashRing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CDashRing::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDashRing::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};
	if (pArg != nullptr)
		memcpy(&Desc, pArg, sizeof(GAMEOBJECT_DESC));
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	if (nullptr == pArg)
	{
		lstrcpy(Desc.szName, TEXT("DashRing"));
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

HRESULT CDashRing::Initialize(simdjson::ondemand::object& jsonData)
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

HRESULT CDashRing::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Model_DashRing"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CRigidBody::RIGIDBODY_DESC rigidBodyDesc = {};
	rigidBodyDesc.vLocalPosition = { 0.f, 0.0f, 0.f };
	CRigidBody::SHAPE_DESC shapeDesc = {};
	shapeDesc.eShapeType = CRigidBody::EShapeType::SPHERE;
	shapeDesc.vShapeSize = { 1.f, 1.f, 1.f };
	rigidBodyDesc.vShapeDescs.push_back(shapeDesc);
	rigidBodyDesc.eBodyType = CRigidBody::EBodyType::STATIC;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &rigidBodyDesc)))
		return E_FAIL;


	return S_OK;
}

void CDashRing::Free()
{
	__super::Free();
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}

#pragma endregion

#pragma region Save&Load

HRESULT CDashRing::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CDashRing"] = data;
	outJson["CDashRing"]["PrototypeTag"] = "Prototype_GameObject_DashRing";
	return S_OK;
}

HRESULT CDashRing::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

HRESULT CDashRing::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CDashRing";
	return S_OK;
}

HRESULT CDashRing::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma endregion

#pragma region Loops
void CDashRing::Priority_Update(_float fTimeDelta)
{

}

void CDashRing::Update(_float fTimeDelta)
{
	if (m_pRigidBodyCom)
		m_pRigidBodyCom->Update(fTimeDelta);
}

void CDashRing::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CDashRing::Bind_ShaderResources()
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


HRESULT CDashRing::Render()
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
void CDashRing::OnCollisionEnter(PxActor* pOther, const PxContactPair& cp)
{
	if (m_pGameInstance->Get_CurrentPlayer() == pOther->userData)
	{
		CGameObject* pPlayer = static_cast<CGameObject*>(pOther->userData);
		CSonic_Controller* pController = static_cast<CSonic_Controller*>(pPlayer->Get_Component(L"Com_Controller"));
		CTransform* pPlayerTransform = static_cast<CTransform*>(pPlayer->Get_Component(g_strTransformTag));

		if (pController != nullptr && pPlayerTransform != nullptr)
		{
			_vector vPlayerPos = pPlayerTransform->Get_State(STATE::POSITION);
			_vector vRingPos = m_pTransformCom->Get_State(STATE::POSITION);
			_vector vRingLook = m_pTransformCom->Get_State(STATE::LOOK);

			// 링의 중심에서 플레이어로 향하는 벡터
			_vector vToPlayer = vPlayerPos - vRingPos;

			// 링의 Look 벡터와 vToPlayer 벡터를 내적하여 플레이어의 위치 판별
			_float fDot = XMVectorGetX(XMVector3Dot(vRingLook, vToPlayer));

			_vector vDashDirection = vRingLook;
			// fDot이 양수이면 플레이어는 링의 뒤쪽에 있으므로, -Look 방향으로 대시해야 링을 통과함
			if (fDot > 0.f)
			{
				vDashDirection = -vRingLook;
			}

			// 플레이어의 현재 이동 방향과 결정된 대시 방향이 유사한지 확인 (역주행 방지)
			_vector vPlayerMoveDir = XMLoadFloat3(&pController->Get_Status().vMoveDirection);
			if (XMVectorGetX(XMVector3Dot(vPlayerMoveDir, vDashDirection)) > 0.f)
			{
				pController->DashRing(vDashDirection);
			}
		}
	}
}
void CDashRing::OnCollisionOverlapping(PxActor* pOther)
{
}
void CDashRing::OnCollisionExit(PxActor* pOther)
{
}
#pragma endregion