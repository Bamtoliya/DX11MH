
#include "RigidBody.h"
#include "GameInstance.h"

#pragma region Constructor/Destructor
CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
	, m_pActor{ nullptr }
	, m_pShape{ nullptr }
	, m_pMaterial{ nullptr }
	, m_bIsLateInitialized{ false }
{
	ZeroMemory(&m_tRigidBodyDesc, sizeof(RIGIDBODY_DESC));
}

CRigidBody::CRigidBody(const CRigidBody& Prototype)
	: CComponent{ Prototype }
	, m_tRigidBodyDesc{ Prototype.m_tRigidBodyDesc }	// 생성 시 디스크립션 복사
	, m_pActor{ nullptr }								// 복제 후 Initialize에서 새로 생성
	, m_pShape{ nullptr }
	, m_pMaterial{ nullptr }
	, m_bIsLateInitialized{ false }
{
}

HRESULT CRigidBody::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CRigidBody::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (pArg == nullptr)
	{
		// 기본값으로 초기화 또는 에러 처리
		MSG_BOX("CRigidBody::Initialize - pArg is nullptr. Using default RIGIDBODY_DESC.");
		// m_tRigidBodyDesc는 이미 생성자에서 ZeroMemory 되었고, 기본값이 설정되어 있을 수 있음.
	}
	else
	{
		m_tRigidBodyDesc = *static_cast<RIGIDBODY_DESC*>(pArg);
	}

	return S_OK;
}
HRESULT CRigidBody::Late_Initialize()
{
	return S_OK;
}

CRigidBody* CRigidBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRigidBody* pInstance = new CRigidBody(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRigidBody");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CRigidBody* CRigidBody::Clone(void* pArg)
{
	CRigidBody* pInstance = new CRigidBody(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CRigidBody");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CRigidBody::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region Save&Load
HRESULT CRigidBody::Serialize(json& data)
{
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	return S_OK;
}
HRESULT CRigidBody::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}
#pragma endregion

#pragma region Loops
void CRigidBody::Priority_Update(_float fTimeDelta)
{
	if (!m_bActive || !m_bIsLateInitialized || !m_pActor)
		return;

	// 게임 오브젝트의 Transform이 변경되었다면 PhysX 액터에 반영
	// (예: 게임 로직에 의해 직접 위치가 변경된 경우, 키네마틱 액터의 타겟 설정 등)
	if (m_tRigidBodyDesc.eBodyType == EBodyType::KINEMATIC || m_bIsTransformDirty)
	{
		Synchronize_Transform_To_Physics();
		m_bIsTransformDirty = false;
	}
}
void CRigidBody::Update(_float fTimeDelta)
{
}
void CRigidBody::Late_Update(_float fTimeDelta)
{
	if (!m_bActive || !m_bIsLateInitialized || !m_pActor)
		return;

	// Dynamic 또는 Kinematic 액터의 경우, 물리 시뮬레이션 결과를 Transform에 반영
	if (m_tRigidBodyDesc.eBodyType == EBodyType::DYNAMIC || m_tRigidBodyDesc.eBodyType == EBodyType::KINEMATIC)
	{
		Synchronize_Physics_To_Transform();
	}
}
HRESULT CRigidBody::Render()
{
	return S_OK;
}
#pragma endregion

#pragma region Debug
HRESULT CRigidBody::RenderImGui()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("RigidBody", ImGuiTreeNodeFlags_DefaultOpen))
	{

	}
	ImGui::PopID();
	return S_OK;
}
#pragma endregion



void CRigidBody::Add_Force(const _float3& vForce, PxForceMode::Enum eForceMode)
{
}

void CRigidBody::Add_Torque(const _float3& vTorque, PxForceMode::Enum eForceMode)
{
}

void CRigidBody::Set_Linear_Velocity(const _float3& vVelocity)
{
}

_float3 CRigidBody::Get_Linear_Velocity() const
{
	return _float3();
}

void CRigidBody::Set_Angular_Velocity(const _float3& vVelocity)
{
}

_float3 CRigidBody::Get_Angular_Velocity() const
{
	return _float3();
}

void CRigidBody::Set_Kinematic_Target(const _float4x4& matTargetWorld)
{
}

void CRigidBody::Set_Body_Type(EBodyType eType)
{
}

void CRigidBody::Set_Mass(_float fMass)
{
}

void CRigidBody::Enable_Gravity(_bool bEnable)
{
}

void CRigidBody::Set_Material_Properties(_float fStaticFriction, _float fDynamicFriction, _float fRestitution)
{
}

void CRigidBody::Lock_Rotation_Axis(_bool bLockX, _bool bLockY, _bool bLockZ)
{
}

HRESULT CRigidBody::Create_Actor()
{
	return E_NOTIMPL;
}

void CRigidBody::Release_Actor()
{
}

void CRigidBody::Synchronize_Transform_To_Physics()
{
}

void CRigidBody::Synchronize_Physics_To_Transform()
{
}
