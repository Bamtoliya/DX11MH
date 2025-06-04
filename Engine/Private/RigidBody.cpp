
#include "RigidBody.h"
#include "Transform.h"
#include "GameInstance.h"

#pragma region Constructor/Destructor
CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
	, m_pActor{ nullptr }
	, m_pShape{ nullptr }
	, m_pMaterial{ nullptr }
	, m_bIsLateInitialized{ false }
{
	//ZeroMemory(&m_tRigidBodyDesc, sizeof(RIGIDBODY_DESC));
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
	Create_Actor();
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
HRESULT CRigidBody::Serialize(json& outJson)
{
	if (FAILED(__super::Serialize(outJson)))
		return E_FAIL;
	json rbData;
	rbData["BodyType"] = static_cast<int>(m_tRigidBodyDesc.eBodyType);
	rbData["ShapeType"] = static_cast<int>(m_tRigidBodyDesc.eShapeType);
	rbData["ShapeSize"] = { m_tRigidBodyDesc.vShapeSize.x, m_tRigidBodyDesc.vShapeSize.y, m_tRigidBodyDesc.vShapeSize.z };
	rbData["LocalPosition"] = { m_tRigidBodyDesc.vLocalPosition.x, m_tRigidBodyDesc.vLocalPosition.y, m_tRigidBodyDesc.vLocalPosition.z };
	rbData["LocalEulerRotation"] = { m_tRigidBodyDesc.fCurRotationX, m_tRigidBodyDesc.fCurRotationY, m_tRigidBodyDesc.fCurRotationZ };
	// qRotation은 fCurRotationX/Y/Z로부터 Create_Actor 시 계산되므로 별도 저장 필요 없음

	rbData["Mass"] = m_tRigidBodyDesc.fMass;
	rbData["Gravity"] = m_tRigidBodyDesc.bGravity;
	rbData["StaticFriction"] = m_tRigidBodyDesc.fStaticFriction;
	rbData["DynamicFriction"] = m_tRigidBodyDesc.fDynamicFriction;
	rbData["Restitution"] = m_tRigidBodyDesc.fRestitution;
	rbData["LockRotX"] = m_tRigidBodyDesc.bLockRotX;
	rbData["LockRotY"] = m_tRigidBodyDesc.bLockRotY;
	rbData["LockRotZ"] = m_tRigidBodyDesc.bLockRotZ;
	rbData["KinematicTarget"] = m_tRigidBodyDesc.bKinematicTarget;
	// pModelForMesh는 현재 직렬화/역직렬화에서 제외 (포인터 직접 저장은 부적절)

	outJson["CRigidBody"] = rbData;
	return S_OK;
}
HRESULT CRigidBody::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;

	simdjson::ondemand::object rbObject;
	if (jsonData["CRigidBody"].get_object().get(rbObject) == simdjson::SUCCESS)
	{
		m_tRigidBodyDesc.eBodyType = static_cast<EBodyType>(int(rbObject["BodyType"].get_int64().value_unsafe()));
		m_tRigidBodyDesc.eShapeType = static_cast<EShapeType>(int(rbObject["ShapeType"].get_int64().value_unsafe()));

		simdjson::ondemand::array shapeSizeArray;
		if (rbObject["ShapeSize"].get_array().get(shapeSizeArray) == simdjson::SUCCESS && shapeSizeArray.count_elements() == 3) {
			auto iter = shapeSizeArray.begin();
			m_tRigidBodyDesc.vShapeSize.x = static_cast<_float>((*iter).get_double().value_unsafe());
			++iter;
			m_tRigidBodyDesc.vShapeSize.y = static_cast<_float>((*iter).get_double().value_unsafe());
			++iter;
			m_tRigidBodyDesc.vShapeSize.z = static_cast<_float>((*iter).get_double().value_unsafe());
		}

		simdjson::ondemand::array localPosArray;
		if (rbObject["LocalPosition"].get_array().get(localPosArray) == simdjson::SUCCESS && localPosArray.count_elements() == 3) {
			auto iter = localPosArray.begin();
			m_tRigidBodyDesc.vLocalPosition.x = static_cast<_float>((*iter).get_double().value_unsafe());
			++iter;
			m_tRigidBodyDesc.vLocalPosition.y = static_cast<_float>((*iter).get_double().value_unsafe());
			++iter;
			m_tRigidBodyDesc.vLocalPosition.z = static_cast<_float>((*iter).get_double().value_unsafe());
		}

		simdjson::ondemand::array localEulerArray;
		if (rbObject["LocalEulerRotation"].get_array().get(localEulerArray) == simdjson::SUCCESS && localEulerArray.count_elements() == 3) {
			auto iter = localEulerArray.begin();
			m_tRigidBodyDesc.fCurRotationX = static_cast<_float>((*iter).get_double().value_unsafe());
			++iter;
			m_tRigidBodyDesc.fCurRotationY = static_cast<_float>((*iter).get_double().value_unsafe());
			++iter;
			m_tRigidBodyDesc.fCurRotationZ = static_cast<_float>((*iter).get_double().value_unsafe());
		}
		// qRotation은 fCurRotationX/Y/Z 로드 후 Create_Actor에서 계산됨

		m_tRigidBodyDesc.fMass = static_cast<_float>(rbObject["Mass"].get_double().value_unsafe());
		m_tRigidBodyDesc.bGravity = rbObject["Gravity"].get_bool().value_unsafe();
		m_tRigidBodyDesc.fStaticFriction = static_cast<_float>(rbObject["StaticFriction"].get_double().value_unsafe());
		m_tRigidBodyDesc.fDynamicFriction = static_cast<_float>(rbObject["DynamicFriction"].get_double().value_unsafe());
		m_tRigidBodyDesc.fRestitution = static_cast<_float>(rbObject["Restitution"].get_double().value_unsafe());
		m_tRigidBodyDesc.bLockRotX = rbObject["LockRotX"].get_bool().value_unsafe();
		m_tRigidBodyDesc.bLockRotY = rbObject["LockRotY"].get_bool().value_unsafe();
		m_tRigidBodyDesc.bLockRotZ = rbObject["LockRotZ"].get_bool().value_unsafe();
		m_tRigidBodyDesc.bKinematicTarget = rbObject["KinematicTarget"].get_bool().value_unsafe();
	}
	else
	{
		// CRigidBody 데이터가 없을 경우 경고 또는 기본값 사용
		MSG_BOX("CRigidBody::Deserialize - CRigidBody data not found in JSON. Using defaults or previously set values.");
	}

	// 역직렬화 후 액터 상태를 업데이트해야 할 수 있음
	// 예를 들어, 이미 Late_Initialize가 호출된 상태에서 Deserialize가 호출된다면,
	// 액터를 재생성하거나 업데이트해야 함.
	//if (m_bIsLateInitialized) // 이미 초기화(액터 생성)가 완료된 경우
	//{
	Release_Actor();
	Create_Actor(); // 새 설정으로 액터 재생성
	//}


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
	ImGui::PushID(this); // 고유 ID 보장
	if (ImGui::CollapsingHeader("RigidBody", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// Body Type
		const char* bodyTypes[] = { "Static", "Dynamic", "Kinematic" };
		int currentBodyType = static_cast<int>(m_tRigidBodyDesc.eBodyType);
		if (ImGui::Combo("Body Type", &currentBodyType, bodyTypes, IM_ARRAYSIZE(bodyTypes)))
		{
			Set_Body_Type(static_cast<EBodyType>(currentBodyType)); // 변경 시 액터 재생성
		}

		// Shape Type
		const char* shapeTypes[] = { "Box", "Sphere", "Capsule", "Convex Mesh", "Triangle Mesh" };
		int currentShapeType = static_cast<int>(m_tRigidBodyDesc.eShapeType);
		if (ImGui::Combo("Shape Type", &currentShapeType, shapeTypes, IM_ARRAYSIZE(shapeTypes)))
		{
			// Shape Type 변경 시에도 액터 재생성이 필요할 수 있음 (현재 Set_Body_Type에서 처리)
			// 별도의 Set_Shape_Type 함수를 만들거나, 여기서 직접 Release_Actor/Create_Actor 호출 고려
			EBodyType prevBodyType = m_tRigidBodyDesc.eBodyType; // 임시 저장
			Release_Actor();
			m_tRigidBodyDesc.eShapeType = static_cast<EShapeType>(currentShapeType);
			Create_Actor();
			// 만약 Create_Actor가 실패하면 이전 타입으로 복구하는 로직이 필요할 수 있음
			// 또는, Set_Body_Type처럼 내부에서 처리하도록 함수화
		}

		// Shape Size (Shape Type에 따라 다르게 표시)
		// TODO: EShapeType에 따라 vShapeSize의 의미가 달라지므로, 레이블 등을 적절히 변경
		if (ImGui::DragFloat3("Shape Size", &m_tRigidBodyDesc.vShapeSize.x, 0.1f))
		{
			// 크기 변경 시 액터 재생성 필요
			Release_Actor();
			Create_Actor();
		}

		if (ImGui::DragFloat3("Local Position", &m_tRigidBodyDesc.vLocalPosition.x, 0.1f))
		{
			Release_Actor(); // 로컬 위치 변경 시 액터 즉시 재생성
			Create_Actor();
			// m_bIsTransformDirty = true; // 또는 다음 프레임에 동기화
		}

		ImGui::Text("Box:(HalfExtents), Sphere:(Radius,0,0), Capsule:(Radius,HalfHeight,0)");

		// Rotation (Euler Angles from m_tRigidBodyDesc.qRotation)
		// 이 부분은 m_tRigidBodyDesc.qRotation을 오일러 각도로 변환하여 표시하고,
		// ImGui에서 오일러 각도를 수정하면 m_tRigidBodyDesc.qRotation을 업데이트하고 액터를 재생성합니다.
		// Create_Actor에서는 m_tRigidBodyDesc.fCurRotationX/Y/Z를 사용하여 m_tRigidBodyDesc.qRotation을 설정하므로,
		// ImGui는 항상 최신의 qRotation을 기반으로 작동합니다.

		_float3 eulerAnglesDegrees = {
			XMConvertToDegrees(m_tRigidBodyDesc.fCurRotationX),
			XMConvertToDegrees(m_tRigidBodyDesc.fCurRotationY),
			XMConvertToDegrees(m_tRigidBodyDesc.fCurRotationZ)
		};

		if (ImGui::DragFloat3(u8"Rotation (Euler °)", &eulerAnglesDegrees.x, 1.0f, -360.f, 360.f))
		{
			_float3 newEulerAnglesRadians = {
				XMConvertToRadians(eulerAnglesDegrees.x), // Roll
				XMConvertToRadians(eulerAnglesDegrees.y), // Pitch
				XMConvertToRadians(eulerAnglesDegrees.z)  // Yaw
			};
			// XMQuaternionRotationRollPitchYaw expects Pitch, Yaw, Roll order
			XMVECTOR newQuaternion = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&_float3(m_tRigidBodyDesc.fCurRotationX, m_tRigidBodyDesc.fCurRotationY, m_tRigidBodyDesc.fCurRotationZ)));
			XMStoreFloat4(&m_tRigidBodyDesc.qRotation, XMQuaternionNormalize(newQuaternion));

			// ImGui에서 변경 시 fCurRotationX/Y/Z도 동기화 (선택 사항, Create_Actor에서만 사용된다면 불필요할 수 있음)
			m_tRigidBodyDesc.fCurRotationX = newEulerAnglesRadians.x; // Roll
			m_tRigidBodyDesc.fCurRotationY = newEulerAnglesRadians.y; // Pitch
			m_tRigidBodyDesc.fCurRotationZ = newEulerAnglesRadians.z; // Yaw

			Release_Actor();
			Create_Actor();
		}


		//if (m_tRigidBodyDesc.eShapeType == EShapeType::CONVEX_MESH || m_tRigidBodyDesc.eShapeType == EShapeType::TRIANGLE_MESH)
		//{
		//	ImGui::Checkbox("Is Convex (for Static Mesh)", &m_tRigidBodyDesc.bIsConvex); // bIsConvex가 RigidBody.h에 추가되었다고 가정
		//	// pModelForMesh는 ImGui로 직접 설정하기 어려움 (파일 경로 입력 또는 드래그앤드롭 필요)
		//	ImGui::Text("Model for Mesh: %s", m_tRigidBodyDesc.pModelForMesh ? "Set" : "None");
		//}


		// Dynamic Body 속성들
		if (m_tRigidBodyDesc.eBodyType == EBodyType::DYNAMIC)
		{
			if (ImGui::DragFloat("Mass", &m_tRigidBodyDesc.fMass, 0.1f, 0.01f, 1000.f))
			{
				Set_Mass(m_tRigidBodyDesc.fMass);
			}
			bool bGravity = m_tRigidBodyDesc.bGravity;
			if (ImGui::Checkbox("Enable Gravity", &bGravity))
			{
				Enable_Gravity(bGravity);
			}

			ImGui::Text("Rotation Lock:");
			ImGui::Indent();
			bool bLockX = m_tRigidBodyDesc.bLockRotX;
			bool bLockY = m_tRigidBodyDesc.bLockRotY;
			bool bLockZ = m_tRigidBodyDesc.bLockRotZ;
			if (ImGui::Checkbox("Lock X", &bLockX)) Lock_Rotation_Axis(bLockX, bLockY, bLockZ); ImGui::SameLine();
			if (ImGui::Checkbox("Lock Y", &bLockY)) Lock_Rotation_Axis(bLockX, bLockY, bLockZ); ImGui::SameLine();
			if (ImGui::Checkbox("Lock Z", &bLockZ)) Lock_Rotation_Axis(bLockX, bLockY, bLockZ);
			ImGui::Unindent();
		}

		// Material Properties
		if (ImGui::TreeNode("Material Properties"))
		{
			bool bMaterialChanged = false;
			bMaterialChanged |= ImGui::DragFloat("Static Friction", &m_tRigidBodyDesc.fStaticFriction, 0.01f, 0.f, 10.f);
			bMaterialChanged |= ImGui::DragFloat("Dynamic Friction", &m_tRigidBodyDesc.fDynamicFriction, 0.01f, 0.f, 10.f);
			bMaterialChanged |= ImGui::DragFloat("Restitution", &m_tRigidBodyDesc.fRestitution, 0.01f, 0.f, 1.f);
			if (bMaterialChanged)
			{
				Set_Material_Properties(m_tRigidBodyDesc.fStaticFriction, m_tRigidBodyDesc.fDynamicFriction, m_tRigidBodyDesc.fRestitution);
			}
			ImGui::TreePop();
		}

		// 현재 속도 정보 (읽기 전용)
		if (m_pActor && (m_tRigidBodyDesc.eBodyType == EBodyType::DYNAMIC || m_tRigidBodyDesc.eBodyType == EBodyType::KINEMATIC))
		{
			_float3 linearVel = Get_Linear_Velocity();
			_float3 angularVel = Get_Angular_Velocity();
			ImGui::Text("Linear Velocity: (%.2f, %.2f, %.2f)", linearVel.x, linearVel.y, linearVel.z);
			ImGui::Text("Angular Velocity: (%.2f, %.2f, %.2f)", angularVel.x, angularVel.y, angularVel.z);
		}
	}
	ImGui::PopID();
	return S_OK;
}
#pragma endregion

namespace { // 익명 네임스페이스 사용
	physx::PxTransform ToPxTransform(const _float4x4& matrix) {
		physx::PxMat44 pxMat(reinterpret_cast<const physx::PxMat44&>(matrix));
		return physx::PxTransform(pxMat);
	}

	_float4x4 ToFloat4x4(const physx::PxTransform& transform) {
		_float4x4 matrix;
		XMStoreFloat4x4(&matrix, XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&physx::PxMat44(transform))));
		return matrix;
	}

	physx::PxVec3 ToPxVec3(const _float3& vec) {
		return physx::PxVec3(vec.x, vec.y, vec.z);
	}
	_float3 ToFloat3(const physx::PxVec3& vec) {
		return _float3(vec.x, vec.y, vec.z);
	}

	_float4 ToFloat4(const PxQuat& quat)
	{
		return _float4(quat.x, quat.y, quat.z, quat.z);
	}

	PxQuat ToPxQuat(const _float4& quat) {
		return PxQuat(quat.x, quat.y, quat.z, quat.w);
	}
}

#pragma region Physics
void CRigidBody::Add_Force(const _float3& vForce, PxForceMode::Enum eForceMode)
{
	if (m_pActor && m_tRigidBodyDesc.eBodyType == EBodyType::DYNAMIC)
	{
		physx::PxRigidDynamic* pDynamicActor = static_cast<physx::PxRigidDynamic*>(m_pActor);
		if (pDynamicActor)
		{
			pDynamicActor->addForce(ToPxVec3(vForce), eForceMode);
		}
	}
}

void CRigidBody::Add_Torque(const _float3& vTorque, PxForceMode::Enum eForceMode)
{
	if (m_pActor && m_tRigidBodyDesc.eBodyType == EBodyType::DYNAMIC)
	{
		physx::PxRigidDynamic* pDynamicActor = static_cast<physx::PxRigidDynamic*>(m_pActor);
		if (pDynamicActor)
		{
			pDynamicActor->addTorque(ToPxVec3(vTorque), eForceMode);
		}
	}
}

void CRigidBody::Set_Linear_Velocity(const _float3& vVelocity)
{
	if (m_pActor && (m_tRigidBodyDesc.eBodyType == EBodyType::DYNAMIC || m_tRigidBodyDesc.eBodyType == EBodyType::KINEMATIC))
	{
		physx::PxRigidBody* pBody = static_cast<physx::PxRigidBody*>(m_pActor);
		if (pBody)
		{
			//pBody->setLinearVelocity(ToPxVec3(vVelocity));
		}
	}
}

_float3 CRigidBody::Get_Linear_Velocity() const
{
	if (m_pActor && (m_tRigidBodyDesc.eBodyType == EBodyType::DYNAMIC || m_tRigidBodyDesc.eBodyType == EBodyType::KINEMATIC))
	{
		physx::PxRigidBody* pBody = static_cast<physx::PxRigidBody*>(m_pActor);
		if (pBody)
		{
			return ToFloat3(pBody->getLinearVelocity());
		}
	}
	return _float3(0.f, 0.f, 0.f);
}

void CRigidBody::Set_Angular_Velocity(const _float3& vVelocity)
{
	if (m_pActor && (m_tRigidBodyDesc.eBodyType == EBodyType::DYNAMIC || m_tRigidBodyDesc.eBodyType == EBodyType::KINEMATIC))
	{
		physx::PxRigidBody* pBody = static_cast<physx::PxRigidBody*>(m_pActor);
		if (pBody)
		{
			//pBody->setAngularVelocity(ToPxVec3(vVelocity));
		}
	}
}

_float3 CRigidBody::Get_Angular_Velocity() const
{
	if (m_pActor && (m_tRigidBodyDesc.eBodyType == EBodyType::DYNAMIC || m_tRigidBodyDesc.eBodyType == EBodyType::KINEMATIC))
	{
		physx::PxRigidBody* pBody = static_cast<physx::PxRigidBody*>(m_pActor);
		if (pBody)
		{
			return ToFloat3(pBody->getAngularVelocity());
		}
	}
	return _float3(0.f, 0.f, 0.f);
}

void CRigidBody::Set_Kinematic_Target(const _float4x4& matTargetWorld)
{
	if (m_pActor && m_tRigidBodyDesc.eBodyType == EBodyType::KINEMATIC)
	{
		physx::PxRigidDynamic* pKinematicActor = static_cast<physx::PxRigidDynamic*>(m_pActor);
		if (pKinematicActor)
		{
			pKinematicActor->setKinematicTarget(ToPxTransform(matTargetWorld));
		}
	}
}

void CRigidBody::Set_Body_Type(EBodyType eType)
{
	if (m_tRigidBodyDesc.eBodyType == eType)
		return;

	m_tRigidBodyDesc.eBodyType = eType;
	// 액터 타입 변경 시 기존 액터 해제 후 새로 생성
	Release_Actor();
	Create_Actor(); // m_bIsLateInitialized는 이미 true일 것이므로, 즉시 생성
}

void CRigidBody::Set_Mass(_float fMass)
{
	if (m_pActor && m_tRigidBodyDesc.eBodyType == EBodyType::DYNAMIC)
	{
		physx::PxRigidDynamic* pDynamicActor = static_cast<physx::PxRigidDynamic*>(m_pActor);
		if (pDynamicActor)
		{
			m_tRigidBodyDesc.fMass = fMass;
			pDynamicActor->setMass(fMass);
		}
	}
}

void CRigidBody::Enable_Gravity(_bool bEnable)
{
	if (m_pActor)
	{
		m_tRigidBodyDesc.bGravity = bEnable;
		m_pActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !bEnable);
	}
}

void CRigidBody::Set_Material_Properties(_float fStaticFriction, _float fDynamicFriction, _float fRestitution)
{
	if (m_pMaterial)
	{
		m_tRigidBodyDesc.fStaticFriction = fStaticFriction;
		m_tRigidBodyDesc.fDynamicFriction = fDynamicFriction;
		m_tRigidBodyDesc.fRestitution = fRestitution;
		m_pMaterial->setStaticFriction(fStaticFriction);
		m_pMaterial->setDynamicFriction(fDynamicFriction);
		m_pMaterial->setRestitution(fRestitution);
	}
}

void CRigidBody::Lock_Rotation_Axis(_bool bLockX, _bool bLockY, _bool bLockZ)
{
	if (m_pActor && m_tRigidBodyDesc.eBodyType == EBodyType::DYNAMIC)
	{
		physx::PxRigidDynamic* pDynamicActor = static_cast<physx::PxRigidDynamic*>(m_pActor);
		if (pDynamicActor)
		{
			m_tRigidBodyDesc.bLockRotX = bLockX;
			m_tRigidBodyDesc.bLockRotY = bLockY;
			m_tRigidBodyDesc.bLockRotZ = bLockZ;

			physx::PxRigidDynamicLockFlags flags;
			if (bLockX) flags |= physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
			if (bLockY) flags |= physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
			if (bLockZ) flags |= physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;

			pDynamicActor->setRigidDynamicLockFlags(flags);
		}
	}
}

HRESULT CRigidBody::Create_Actor()
{
	PxPhysics* pPhysics = m_pGameInstance->Get_Physics(); // CGameInstance에 Get_PhysX_Physics()가 있다고 가정
	PxScene* pScene = m_pGameInstance->Get_PhysicsScene();       // CGameInstance에 Get_PhysX_Scene()가 있다고 가정

	CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	if (pTransform == nullptr)
		return E_FAIL;

	// 1. Material 생성
	m_pMaterial = pPhysics->createMaterial(m_tRigidBodyDesc.fStaticFriction, m_tRigidBodyDesc.fDynamicFriction, m_tRigidBodyDesc.fRestitution);
	if (!m_pMaterial)
		return E_FAIL;
	

	// 2. Shape Geometry 생성
	PxGeometry* pGeometry = nullptr;
	PxTriangleMesh* triangleMesh = nullptr; // Triangle Mesh 리소스
	PxConvexMesh* convexMesh = nullptr;     // Convex Mesh 리소스
	switch (m_tRigidBodyDesc.eShapeType)
	{
	case EShapeType::BOX:
		pGeometry = new physx::PxBoxGeometry(m_tRigidBodyDesc.vShapeSize.x, m_tRigidBodyDesc.vShapeSize.y, m_tRigidBodyDesc.vShapeSize.z);
		break;
	case EShapeType::SPHERE:
		pGeometry = new physx::PxSphereGeometry(m_tRigidBodyDesc.vShapeSize.x); // radius
		break;
	case EShapeType::CAPSULE:
		pGeometry = new physx::PxCapsuleGeometry(m_tRigidBodyDesc.vShapeSize.x, m_tRigidBodyDesc.vShapeSize.y); // radius, halfHeight
		break;
	case EShapeType::TRIANGLE_MESH:
	case EShapeType::CONVEX_MESH:
	{
		//if (!m_tRigidBodyDesc.pModelForMesh) // RigidBody.h에서 pModelForMesh 주석 해제 및 값 설정 필요
		//{
		//	MSG_BOX("CRigidBody::Create_Actor - pModelForMesh is null for MESH shape type.");
		//	m_pMaterial->release(); m_pMaterial = nullptr;
		//	return E_FAIL;
		//}

		//PxVec3* verticesData = nullptr;
		//PxU32 numVertices = 0;
		//void* indicesData = nullptr;
		//PxU32 numIndices = 0;
		//PxMeshFlag::Enum indexFormat = PxMeshFlag::Enum(0);

		//// CModel에서 정점 및 인덱스 데이터 가져오기 (가정)
		//// 실제 모델/메시 구조에 맞게 이 부분을 수정해야 합니다.
		//// 예시: 모델의 모든 메시를 병합하거나 첫 번째 메시만 사용
		//std::vector<physx::PxVec3> pxVertices;
		//std::vector<PxU32> pxIndices; // Triangle Mesh에만 필요

		//// --- 모델 데이터 로드 시작 (플레이스홀더) ---
		//// 이 부분은 프로젝트의 CModel, CMesh 구조에 따라 실제 데이터를 로드해야 합니다.
		//// 예를 들어, m_tRigidBodyDesc.pModelForMesh->Get_Meshes() 등으로 메시 목록을 가져오고,
		//// 각 CMesh에서 Get_Vertices_Position(), Get_Indices() 등으로 데이터를 가져와 pxVertices, pxIndices에 채웁니다.
		//// 여기서는 간단한 예시로 하나의 메시에서 데이터를 가져온다고 가정합니다.
		//// CModel* pModel = m_tRigidBodyDesc.pModelForMesh;
		//// if (pModel->Get_MeshCount() > 0) {
		////    CMesh* pMesh = pModel->Get_Mesh(0); // 첫 번째 메시 사용 가정
		////    const auto& modelVertices = pMesh->Get_Vertices(); // _float3 타입의 정점 배열 가정
		////    for(const auto& vert : modelVertices) {
		////        pxVertices.push_back(ToPxVec3(vert.vPosition)); // 정점 구조체에 vPosition이 있다고 가정
		////    }
		////    if (m_tRigidBodyDesc.eShapeType == EShapeType::TRIANGLE_MESH && !m_tRigidBodyDesc.bIsConvex) { // bIsConvex는 RigidBody.h에 추가 필요
		////        const auto& modelIndices = pMesh->Get_Indices32(); // _uint 타입의 인덱스 배열 가정
		////        pxIndices.insert(pxIndices.end(), modelIndices.begin(), modelIndices.end());
		////    }
		//// }
		//// --- 임시 플레이스홀더 데이터 (실제로는 모델에서 가져와야 함) ---
		//// 예: 큐브 정점 데이터 (Convex 및 Triangle Mesh 공통)
		//pxVertices.push_back(physx::PxVec3(-0.5f, -0.5f, -0.5f) * ToPxVec3(m_tRigidBodyDesc.vShapeSize));
		//pxVertices.push_back(physx::PxVec3(0.5f, -0.5f, -0.5f) * ToPxVec3(m_tRigidBodyDesc.vShapeSize));
		//pxVertices.push_back(physx::PxVec3(-0.5f, 0.5f, -0.5f) * ToPxVec3(m_tRigidBodyDesc.vShapeSize));
		//pxVertices.push_back(physx::PxVec3(0.5f, 0.5f, -0.5f) * ToPxVec3(m_tRigidBodyDesc.vShapeSize));
		//pxVertices.push_back(physx::PxVec3(-0.5f, -0.5f, 0.5f) * ToPxVec3(m_tRigidBodyDesc.vShapeSize));
		//pxVertices.push_back(physx::PxVec3(0.5f, -0.5f, 0.5f) * ToPxVec3(m_tRigidBodyDesc.vShapeSize));
		//pxVertices.push_back(physx::PxVec3(-0.5f, 0.5f, 0.5f) * ToPxVec3(m_tRigidBodyDesc.vShapeSize));
		//pxVertices.push_back(physx::PxVec3(0.5f, 0.5f, 0.5f) * ToPxVec3(m_tRigidBodyDesc.vShapeSize));
		//// --- 임시 플레이스홀더 데이터 끝 ---


		//if (pxVertices.empty()) {
		//	MSG_BOX("CRigidBody::Create_Actor - No vertices found for MESH shape type.");
		//	m_pMaterial->release(); m_pMaterial = nullptr;
		//	return E_FAIL;
		//}

		//// 동적 액터는 항상 Convex Mesh를 사용해야 합니다.
		//// 정적 액터는 Triangle Mesh 또는 Convex Mesh를 사용할 수 있습니다.
		//// m_tRigidBodyDesc.bIsConvex 플래그를 사용하여 정적일 때 어떤 메시를 사용할지 결정할 수 있습니다.
		//bool bForceConvex = (m_tRigidBodyDesc.eBodyType == EBodyType::DYNAMIC || m_tRigidBodyDesc.eBodyType == EBodyType::KINEMATIC);

		//if (m_tRigidBodyDesc.eShapeType == EShapeType::TRIANGLE_MESH && !bForceConvex && !m_tRigidBodyDesc.bIsConvex)
		//{
		//	// --- 임시 플레이스홀더 인덱스 데이터 (실제로는 모델에서 가져와야 함) ---
		//	// 큐브 인덱스 데이터 (Triangle List)
		//	PxU32 inds[] = {
		//		0,2,1, 1,2,3, 4,5,6, 5,7,6, 0,1,5, 0,5,4,
		//		2,6,7, 2,7,3, 0,4,2, 2,4,6, 1,3,7, 1,7,5
		//	};
		//	pxIndices.assign(inds, inds + _countof(inds));
		//	// --- 임시 플레이스홀더 인덱스 데이터 끝 ---

		//	if (pxIndices.empty()) {
		//		MSG_BOX("CRigidBody::Create_Actor - No indices found for TRIANGLE_MESH shape type.");
		//		m_pMaterial->release(); m_pMaterial = nullptr;
		//		return E_FAIL;
		//	}

		//	physx::PxTriangleMeshDesc meshDesc;
		//	meshDesc.points.count = static_cast<PxU32>(pxVertices.size());
		//	meshDesc.points.stride = sizeof(physx::PxVec3);
		//	meshDesc.points.data = pxVertices.data();

		//	meshDesc.triangles.count = static_cast<PxU32>(pxIndices.size() / 3);
		//	meshDesc.triangles.stride = 3 * sizeof(PxU32);
		//	meshDesc.triangles.data = pxIndices.data();
		//	// 만약 16비트 인덱스를 사용한다면:
		//	// meshDesc.flags = PxMeshFlag::e16_BIT_INDICES;
		//	// meshDesc.triangles.stride = 3 * sizeof(PxU16);

		//	physx::PxDefaultMemoryOutputStream writeBuffer;
		//	physx::PxTriangleMeshCookingResult::Enum cookResult;
		//	if (!pCooking->cookTriangleMesh(meshDesc, writeBuffer, &cookResult))
		//	{
		//		MSG_BOX("CRigidBody::Create_Actor - Failed to cook triangle mesh.");
		//		m_pMaterial->release(); m_pMaterial = nullptr;
		//		return E_FAIL;
		//	}
		//	physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		//	triangleMesh = pPhysics->createTriangleMesh(readBuffer);
		//	if (!triangleMesh) {
		//		MSG_BOX("CRigidBody::Create_Actor - Failed to create PxTriangleMesh from cooked data.");
		//		m_pMaterial->release(); m_pMaterial = nullptr;
		//		return E_FAIL;
		//	}
		//	// 스케일 적용은 PxTriangleMeshGeometry의 PxMeshScale에서 수행
		//	pGeometry = new physx::PxTriangleMeshGeometry(triangleMesh /*, PxMeshScale(ToPxVec3(m_tRigidBodyDesc.vShapeSize))*/);
		//	// vShapeSize를 정점 데이터에 미리 곱했으므로 여기서는 스케일 1.0으로 처리하거나,
		//	// 정점 데이터는 원본으로 두고 여기서 PxMeshScale을 사용합니다.
		//	// 현재 코드는 정점에 미리 곱하는 방식을 사용했습니다.
		//}
		//else // CONVEX_MESH 또는 Dynamic/Kinematic Triangle Mesh (Convex로 강제)
		//{
		//	physx::PxConvexMeshDesc convexDesc;
		//	convexDesc.points.count = static_cast<PxU32>(pxVertices.size());
		//	convexDesc.points.stride = sizeof(physx::PxVec3);
		//	convexDesc.points.data = pxVertices.data();
		//	convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX; // 볼록 헐 계산
		//	// | physx::PxConvexFlag::eINFLATE_CONVEX; // 필요시 사용
		//	// | physx::PxConvexFlag::eSHIFT_VERTICES; // 원점으로 이동시킬 경우

		//	physx::PxDefaultMemoryOutputStream writeBuffer;
		//	physx::PxConvexMeshCookingResult::Enum cookResult;
		//	if (!pCooking->cookConvexMesh(convexDesc, writeBuffer, &cookResult))
		//	{
		//		MSG_BOX("CRigidBody::Create_Actor - Failed to cook convex mesh.");
		//		m_pMaterial->release(); m_pMaterial = nullptr;
		//		return E_FAIL;
		//	}
		//	physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		//	convexMesh = pPhysics->createConvexMesh(readBuffer);
		//	if (!convexMesh) {
		//		MSG_BOX("CRigidBody::Create_Actor - Failed to create PxConvexMesh from cooked data.");
		//		m_pMaterial->release(); m_pMaterial = nullptr;
		//		return E_FAIL;
		//	}
		//	// 스케일 적용은 PxConvexMeshGeometry의 PxMeshScale에서 수행
		//	pGeometry = new physx::PxConvexMeshGeometry(convexMesh /*, PxMeshScale(ToPxVec3(m_tRigidBodyDesc.vShapeSize))*/);
		//	// vShapeSize를 정점 데이터에 미리 곱했으므로 여기서는 스케일 1.0으로 처리하거나,
		//	// 정점 데이터는 원본으로 두고 여기서 PxMeshScale을 사용합니다.
		//	// 현재 코드는 정점에 미리 곱하는 방식을 사용했습니다.
		//}
	}
		break;
		// TODO: MESH 타입 처리 (PxTriangleMeshGeometry 또는 PxConvexMeshGeometry)
	default:
		return E_FAIL; // 지원하지 않는 모양 타입
	}

	if (!pGeometry) {
		m_pMaterial->release();
		m_pMaterial = nullptr;
		return E_FAIL;
	}

	// 3. Actor 생성
	// CTransform의 월드 정보 가져오기
	_matrix cTransformWorldMatrix = pTransform->Get_WorldMatrix();

	_float3 rbLocalPos = m_tRigidBodyDesc.vLocalPosition;
	XMVECTOR rbLocalRotQuatVec = XMQuaternionRotationRollPitchYaw(
		m_tRigidBodyDesc.fCurRotationY, // Pitch
		m_tRigidBodyDesc.fCurRotationZ, // Yaw
		m_tRigidBodyDesc.fCurRotationX  // Roll
	);
	rbLocalRotQuatVec = XMQuaternionNormalize(rbLocalRotQuatVec);
	XMStoreFloat4(&m_tRigidBodyDesc.qRotation, rbLocalRotQuatVec); // Update desc's qRotation

	_matrix rbLocalTranslationMatrix = XMMatrixTranslation(rbLocalPos.x, rbLocalPos.y, rbLocalPos.z);
	_matrix rbLocalRotationMatrix = XMMatrixRotationQuaternion(rbLocalRotQuatVec);
	_matrix rbLocalMatrix = rbLocalRotationMatrix * rbLocalTranslationMatrix;

	_matrix finalActorWorldMatrix = rbLocalMatrix * cTransformWorldMatrix;

	_vector scale, rot, trans;
	XMMatrixDecompose(&scale, &rot, &trans, finalActorWorldMatrix);

	_float3 finalActorPos; XMStoreFloat3(&finalActorPos, trans);
	_float4 finalActorRot; XMStoreFloat4(&finalActorRot, rot);

	PxTransform actorPose(ToPxVec3(finalActorPos), ToPxQuat(finalActorRot));

	switch (m_tRigidBodyDesc.eBodyType)
	{
	case EBodyType::STATIC:
		m_pActor = pPhysics->createRigidStatic(actorPose);
		break;
	case EBodyType::DYNAMIC:
		m_pActor = pPhysics->createRigidDynamic(actorPose);
		if (m_pActor)
		{
			physx::PxRigidDynamic* pDynamicActor = static_cast<physx::PxRigidDynamic*>(m_pActor);
			pDynamicActor->setMass(m_tRigidBodyDesc.fMass);
			pDynamicActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !m_tRigidBodyDesc.bGravity);

			physx::PxRigidDynamicLockFlags flags;
			if (m_tRigidBodyDesc.bLockRotX) flags |= physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
			if (m_tRigidBodyDesc.bLockRotY) flags |= physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
			if (m_tRigidBodyDesc.bLockRotZ) flags |= physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
			pDynamicActor->setRigidDynamicLockFlags(flags);
		}
		break;
	case EBodyType::KINEMATIC:
		m_pActor = pPhysics->createRigidDynamic(actorPose);
		if (m_pActor)
		{
			static_cast<physx::PxRigidDynamic*>(m_pActor)->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
		}
		break;
	default:
		delete pGeometry; // 생성된 geometry 해제
		m_pMaterial->release();
		m_pMaterial = nullptr;
		return E_FAIL; // 지원하지 않는 바디 타입
	}

	if (!m_pActor)
	{
		delete pGeometry;
		m_pMaterial->release();
		m_pMaterial = nullptr;
		return E_FAIL;
	}

	// 4. Shape 생성 및 Actor에 부착
	m_pShape = physx::PxRigidActorExt::createExclusiveShape(*m_pActor, *pGeometry, *m_pMaterial);
	if (!m_pShape)
	{
		m_pActor->release();
		m_pActor = nullptr;
		m_pMaterial->release();
		m_pMaterial = nullptr;
		return E_FAIL;
	}

	delete pGeometry; // Geometry는 Shape 생성 시 복사되므로 원본은 삭제

	// 5. Actor를 Scene에 추가
	m_pActor->userData = this; // 액터의 userData에 CRigidBody 인스턴스 포인터 저장 (콜백 등에서 사용 가능)
	pScene->addActor(*m_pActor);

	return S_OK;
	return S_OK;
}

void CRigidBody::Release_Actor()
{
	PxScene* pScene = m_pGameInstance->Get_PhysicsScene();
	if (m_pActor && pScene)
	{
		pScene->removeActor(*m_pActor);
	}

	if (m_pShape)
	{
		// Shape은 Actor에 의해 소유되므로 Actor 해제 시 자동으로 해제됨.
		// 명시적으로 release() 호출 불필요. 단, createExclusiveShape가 아닌 경우 별도 관리 필요.
		// 여기서는 createExclusiveShape를 사용했으므로 Actor 해제 시 같이 정리됨.
		m_pShape = nullptr;
	}
	if (m_pActor)
	{
		m_pActor->release();
		m_pActor = nullptr;
	}
	if (m_pMaterial)
	{
		m_pMaterial->release();
		m_pMaterial = nullptr;
	}
}

void CRigidBody::Synchronize_Transform_To_Physics()
{
	if (!m_pActor || !m_pOwner) return;
	CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	if (!pTransform) return;

	_matrix cTransformWorldMatrix = pTransform->Get_WorldMatrix();

	_float3 rbLocalPos = m_tRigidBodyDesc.vLocalPosition;
	XMVECTOR rbLocalRotQuatVec = XMLoadFloat4(&m_tRigidBodyDesc.qRotation); // qRotation is RB's local rotation

	_matrix rbLocalTranslationMatrix = XMMatrixTranslation(rbLocalPos.x, rbLocalPos.y, rbLocalPos.z);
	_matrix rbLocalRotationMatrix = XMMatrixRotationQuaternion(rbLocalRotQuatVec);
	_matrix rbLocalMatrix = rbLocalRotationMatrix * rbLocalTranslationMatrix;

	_matrix finalActorWorldMatrix = rbLocalMatrix * cTransformWorldMatrix;

	_vector s, finalActorRotVec, finalActorPosVec;
	XMMatrixDecompose(&s, &finalActorRotVec, &finalActorPosVec, finalActorWorldMatrix);

	_float3 finalActorPos; XMStoreFloat3(&finalActorPos, finalActorPosVec);
	_float4 finalActorRot; XMStoreFloat4(&finalActorRot, finalActorRotVec);

	PxTransform physxActorWorldPose(ToPxVec3(finalActorPos), ToPxQuat(finalActorRot));

	if (m_tRigidBodyDesc.eBodyType == EBodyType::KINEMATIC)
	{
		static_cast<physx::PxRigidDynamic*>(m_pActor)->setKinematicTarget(physxActorWorldPose);
	}
	else
	{
		m_pActor->setGlobalPose(physxActorWorldPose);
		if (m_tRigidBodyDesc.eBodyType == EBodyType::DYNAMIC)
		{
			physx::PxRigidDynamic* pDynamicActor = static_cast<physx::PxRigidDynamic*>(m_pActor);
			if (pDynamicActor->isSleeping())
			{
				pDynamicActor->wakeUp();
			}
		}
	}
}

void CRigidBody::Synchronize_Physics_To_Transform()
{
	if (!m_pActor || !m_pOwner) return;
	CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	if (!pTransform) return;

	if (m_tRigidBodyDesc.eBodyType == EBodyType::STATIC) return;

	physx::PxTransform physxActorCurrentWorldPose = m_pActor->getGlobalPose();
	_matrix actorWorldMatrix = XMLoadFloat4x4(&ToFloat4x4(physxActorCurrentWorldPose));

	_float3 rbLocalPos = m_tRigidBodyDesc.vLocalPosition;
	XMVECTOR rbLocalRotQuatVec = XMLoadFloat4(&m_tRigidBodyDesc.qRotation); // qRotation is RB's local rotation

	_matrix rbLocalTranslationMatrix = XMMatrixTranslation(rbLocalPos.x, rbLocalPos.y, rbLocalPos.z);
	_matrix rbLocalRotationMatrix = XMMatrixRotationQuaternion(rbLocalRotQuatVec);
	_matrix rbLocalMatrix = rbLocalRotationMatrix * rbLocalTranslationMatrix;

	_matrix invRbLocalMatrix = XMMatrixInverse(nullptr, rbLocalMatrix);
	_matrix targetCTransformWorldMatrix = invRbLocalMatrix * actorWorldMatrix;

	pTransform->Set_WorldMatrix(targetCTransformWorldMatrix);

	// CTransform의 오일러 각도도 동기화 (선택 사항, Set_WorldMatrix 내부에서 처리될 수 있음)
	// pTransform->SynchronizeRotationRepresentations(); // CTransform에 이런 함수가 있다면 호출
}

#pragma endregion