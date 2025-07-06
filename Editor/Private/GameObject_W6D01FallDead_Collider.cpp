#include "GameObject_W6D01FallDead_Collider.h"
#include "GameInstance.h"

#pragma region Constructor/Destructor
CGameObject_W6D01FallDead_Collider::CGameObject_W6D01FallDead_Collider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CGameObject_W6D01FallDead_Collider::CGameObject_W6D01FallDead_Collider(const CGameObject_W6D01FallDead_Collider& Prototype)
	: CGameObject{ Prototype }
{

}
CGameObject_W6D01FallDead_Collider* CGameObject_W6D01FallDead_Collider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameObject_W6D01FallDead_Collider* pInstance = new CGameObject_W6D01FallDead_Collider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGameObject_W6D01FallDead_Collider");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CGameObject_W6D01FallDead_Collider::Clone(void* pArg)
{
	CGameObject_W6D01FallDead_Collider* pInstance = new CGameObject_W6D01FallDead_Collider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGameObject_W6D01FallDead_Collider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGameObject_W6D01FallDead_Collider::Clone(simdjson::ondemand::object& jsonData)
{
	CGameObject_W6D01FallDead_Collider* pInstance = new CGameObject_W6D01FallDead_Collider(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Cloned : CGameObject_W6D01FallDead_Collider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CGameObject_W6D01FallDead_Collider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject_W6D01FallDead_Collider::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};
	if (pArg != nullptr)
		memcpy(&Desc, pArg, sizeof(GAMEOBJECT_DESC));
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	if (nullptr == pArg)
	{
		lstrcpy(Desc.szName, TEXT("GameObject_FallDead_Collider"));
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

HRESULT CGameObject_W6D01FallDead_Collider::Initialize(simdjson::ondemand::object& jsonData)
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

HRESULT CGameObject_W6D01FallDead_Collider::Ready_Components()
{
	/* For.Com_Model_Collider */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Model_W6D01FallDeadCol"),
		TEXT("Com_Model_Collider"), reinterpret_cast<CComponent**>(&m_pModelColliderCom))))
		return E_FAIL;

	CRigidBody::RIGIDBODY_DESC rigidDesc;
	rigidDesc.eBodyType = CRigidBody::EBodyType::STATIC;
	rigidDesc.bIsConvex = false;
	rigidDesc.pModelForMesh = m_pModelColliderCom;
	_uint i = 0;
	for (auto& pMesh : m_pModelColliderCom->Get_Meshes())
	{
		CRigidBody::SHAPE_DESC shapeDesc;
		shapeDesc.pMesh = pMesh;
		shapeDesc.bIsQueryShape = true;
		shapeDesc.iMeshIndex = i++;
		shapeDesc.eShapeType = CRigidBody::EShapeType::TRIANGLE_MESH;
		rigidDesc.vShapeDescs.push_back(shapeDesc);
	}

	/* For.Com_RigidBody */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &rigidDesc)))
		return E_FAIL;


	return S_OK;
}

void CGameObject_W6D01FallDead_Collider::Free()
{
	__super::Free();
	Safe_Release(m_pModelColliderCom);
	Safe_Release(m_pRigidBodyCom);
}

#pragma endregion

#pragma region Save&Load

HRESULT CGameObject_W6D01FallDead_Collider::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CGameObject_W6D01FallDead_Collider"] = data;
	outJson["CGameObject_W6D01FallDead_Collider"]["PrototypeTag"] = "Prototype_GameObject_FallDead_Collider";
	return S_OK;
}

HRESULT CGameObject_W6D01FallDead_Collider::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

HRESULT CGameObject_W6D01FallDead_Collider::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CGameObject_W6D01FallDead_Collider";
	return S_OK;
}

HRESULT CGameObject_W6D01FallDead_Collider::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma endregion

#pragma region Loops
void CGameObject_W6D01FallDead_Collider::Priority_Update(_float fTimeDelta)
{

}

void CGameObject_W6D01FallDead_Collider::Update(_float fTimeDelta)
{

}

void CGameObject_W6D01FallDead_Collider::Late_Update(_float fTimeDelta)
{

}

HRESULT CGameObject_W6D01FallDead_Collider::Render()
{
	if (!m_bVisible)
		return S_OK;

	return S_OK;
}
void CGameObject_W6D01FallDead_Collider::OnCollisionEnter(PxActor* pOther, const PxContactPair& cp)
{
	cout << "Sonic CollidingEnter with Fall Dead\n";
}
void CGameObject_W6D01FallDead_Collider::OnCollisionOverlapping(PxActor* pOther)
{
}
void CGameObject_W6D01FallDead_Collider::OnCollisionExit(PxActor* pOther)
{
}
#pragma endregion