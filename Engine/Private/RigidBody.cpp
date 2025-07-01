#pragma once
#include "RigidBody.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Mesh.h"

#pragma region Constructor/Destructor
CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
	, m_pActor{ nullptr }
	, m_pShapes{ nullptr }
	, m_Materials{ nullptr }
{
	//ZeroMemory(&m_tRigidBodyDesc, sizeof(RIGIDBODY_DESC));
}

CRigidBody::CRigidBody(const CRigidBody& Prototype)
	: CComponent{ Prototype }
	, m_tRigidBodyDesc{ Prototype.m_tRigidBodyDesc }	// 생성 시 디스크립션 복사
	, m_pActor{ nullptr }								// 복제 후 Initialize에서 새로 생성
	, m_pShapes{ nullptr }
	, m_Materials{ nullptr }
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
		MSG_BOX("CRigidBody::Initialize - pArg is nullptr. Using default RIGIDBODY_DESC.");
	}
	else
	{
		m_tRigidBodyDesc = *static_cast<RIGIDBODY_DESC*>(pArg);
	}
	
	return S_OK;
}
HRESULT CRigidBody::Late_Initialize()
{
	Release_Actor();
	Create_Actor();
	m_bLateInitialized = true;
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
	Release_Actor();
}
#pragma endregion

#pragma region Save&Load
HRESULT CRigidBody::Serialize(json& outJson)
{
	if(FAILED(__super::Serialize(outJson)))
		return E_FAIL;

	json rbData;
	rbData["BodyType"] = static_cast<int>(m_tRigidBodyDesc.eBodyType);
	// vActorSize는 이제 vShapeDescs로 관리되므로 직접 직렬화하지 않을 수 있음.
	// 필요하다면, 또는 레거시 호환성을 위해 유지할 수 있으나, 현재 구조에서는 vShapeDescs가 우선.
	// rbData["ActorSize"] = { m_tRigidBodyDesc.vActorSize.x, m_tRigidBodyDesc.vActorSize.y, m_tRigidBodyDesc.vActorSize.z };
	rbData["LocalPosition"] = { m_tRigidBodyDesc.vLocalPosition.x, m_tRigidBodyDesc.vLocalPosition.y, m_tRigidBodyDesc.vLocalPosition.z };
	rbData["LocalRotationQuat"] = { m_tRigidBodyDesc.qRotation.x, m_tRigidBodyDesc.qRotation.y, m_tRigidBodyDesc.qRotation.z, m_tRigidBodyDesc.qRotation.w };
	rbData["LocalEulerRotation"] = { m_tRigidBodyDesc.fCurRotationX, m_tRigidBodyDesc.fCurRotationY, m_tRigidBodyDesc.fCurRotationZ };

	// Material Properties
	rbData["Mass"] = m_tRigidBodyDesc.fMass;
	rbData["Gravity"] = m_tRigidBodyDesc.bGravity;
	rbData["StaticFriction"] = m_tRigidBodyDesc.fStaticFriction;
	rbData["DynamicFriction"] = m_tRigidBodyDesc.fDynamicFriction;
	rbData["Restitution"] = m_tRigidBodyDesc.fRestitution;
	rbData["LockRotX"] = m_tRigidBodyDesc.bLockRotX;
	rbData["LockRotY"] = m_tRigidBodyDesc.bLockRotY;
	rbData["LockRotZ"] = m_tRigidBodyDesc.bLockRotZ;
	rbData["KinematicTarget"] = m_tRigidBodyDesc.bKinematicTarget;
	rbData["IsConvex"] = m_tRigidBodyDesc.bIsConvex; // bIsConvex 추가

	if (m_tRigidBodyDesc.pModelForMesh != nullptr)
	{
		// m_pOwner의 모든 컴포넌트를 순회하여 pModelForMesh와 일치하는 컴포넌트 찾기
		if (m_pOwner)
		{
			// 컴포넌트 이름을 저장할 변수
			wstring modelComponentName = L"";

			// 기본 모델 컴포넌트 확인
			CComponent* pModelComponent = m_pOwner->Get_Component(L"Com_Model");
			if (pModelComponent == m_tRigidBodyDesc.pModelForMesh)
			{
				modelComponentName = L"Com_Model";
			}
			else
			{
				// 다른 이름의 모델 컴포넌트가 있을 수 있음 (예: "Com_Model2", "Com_AlternativeModel" 등)
				// 추가적인 이름 체크가 필요하다면 여기에 구현
			}

			if (!modelComponentName.empty())
			{
				// 찾은 컴포넌트 이름을 저장
				string modelComponentNameUtf8 = WideToUtf8(modelComponentName.c_str());
				rbData["ModelComponentName"] = modelComponentNameUtf8;
			}
		}
	}

	// Shapes 직렬화
	json shapesArray = json::array();
	for (const auto& shapeDesc : m_tRigidBodyDesc.vShapeDescs)
	{
		json shapeData;
		shapeData["ShapeType"] = static_cast<int>(shapeDesc.eShapeType);
		shapeData["ShapeSize"] = { shapeDesc.vShapeSize.x, shapeDesc.vShapeSize.y, shapeDesc.vShapeSize.z };
		shapeData["LocalPosition"] = { shapeDesc.vLocalPosition.x, shapeDesc.vLocalPosition.y, shapeDesc.vLocalPosition.z };
		shapeData["RotationQuat"] = { shapeDesc.qRotation.x, shapeDesc.qRotation.y, shapeDesc.qRotation.z, shapeDesc.qRotation.w };
		shapeData["EulerRotationX"] = shapeDesc.fCurRotationX;
		shapeData["EulerRotationY"] = shapeDesc.fCurRotationY;
		shapeData["EulerRotationZ"] = shapeDesc.fCurRotationZ;
		shapeData["IsQueryShape"] = shapeDesc.bIsQueryShape;
		shapeData["IsDoubleSided"] = shapeDesc.bIsDoubleSided;
		shapeData["FilterData0"] = shapeDesc.filterData0;
		shapeData["FilterData1"] = shapeDesc.filterData1;
		shapeData["FilterData2"] = shapeDesc.filterData2;
		shapeData["FilterData3"] = shapeDesc.filterData3;
		shapeData["MeshIndex"] = shapeDesc.pMesh ? shapeDesc.iMeshIndex : -1; // 메시 이름 저장 (nullptr 체크 필요)
		// shapeDesc.pMesh는 런타임 포인터이므로 직접 직렬화하지 않고,
		// 필요하다면 메시의 이름이나 경로를 저장하고 로드 시점에 다시 연결해야 합니다.
		// 여기서는 생략합니다.
		shapesArray.push_back(shapeData);
	}
	rbData["Shapes"] = shapesArray;

	// pModelForMesh는 런타임 포인터이므로 직접 직렬화하지 않고,
	// 필요하다면 모델의 이름이나 경로를 저장하고 로드 시점에 다시 연결해야 합니다.
	// 여기서는 생략합니다.

	outJson["CRigidBody"] = rbData;
	return S_OK;
}
HRESULT CRigidBody::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;

	simdjson::ondemand::object rbObject;
	if (jsonData["CRigidBody"].get_object().get(rbObject) != simdjson::SUCCESS)
	{
		MSG_BOX("CRigidBody::Deserialize - CRigidBody data not found in JSON. Using defaults or previously set values.");
		return S_FALSE; // 또는 E_FAIL, 정책에 따라
	}

	m_tRigidBodyDesc.eBodyType = static_cast<EBodyType>(int(rbObject["BodyType"].get_int64().value_unsafe()));

	// vActorSize는 Deserialize하지 않거나, 레거시 데이터 처리 로직 추가
	// simdjson::ondemand::array actorSizeArray;
	// if (rbObject["ActorSize"].get_array().get(actorSizeArray) == simdjson::SUCCESS && actorSizeArray.count_elements() == 3) { ... }

	simdjson::ondemand::array localPosArray;
	if (rbObject["LocalPosition"].get_array().get(localPosArray) == simdjson::SUCCESS && localPosArray.count_elements() == 3) {
		auto iter = localPosArray.begin();
		m_tRigidBodyDesc.vLocalPosition.x = static_cast<_float>((*iter).get_double().value_unsafe());
		++iter;
		m_tRigidBodyDesc.vLocalPosition.y = static_cast<_float>((*iter).get_double().value_unsafe());
		++iter;
		m_tRigidBodyDesc.vLocalPosition.z = static_cast<_float>((*iter).get_double().value_unsafe());
	}

	simdjson::ondemand::array localRotQuatArray;
	if (rbObject["LocalRotationQuat"].get_array().get(localRotQuatArray) == simdjson::SUCCESS && localRotQuatArray.count_elements() == 4) {
		auto iter = localRotQuatArray.begin();
		m_tRigidBodyDesc.qRotation.x = static_cast<_float>((*iter).get_double().value_unsafe());
		++iter;
		m_tRigidBodyDesc.qRotation.y = static_cast<_float>((*iter).get_double().value_unsafe());
		++iter;
		m_tRigidBodyDesc.qRotation.z = static_cast<_float>((*iter).get_double().value_unsafe());
		++iter;
		m_tRigidBodyDesc.qRotation.w = static_cast<_float>((*iter).get_double().value_unsafe());
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

	// Material Properties
	m_tRigidBodyDesc.fMass = static_cast<_float>(rbObject["Mass"].get_double().value_unsafe());
	m_tRigidBodyDesc.bGravity = rbObject["Gravity"].get_bool().value_unsafe();
	m_tRigidBodyDesc.fStaticFriction = static_cast<_float>(rbObject["StaticFriction"].get_double().value_unsafe());
	m_tRigidBodyDesc.fDynamicFriction = static_cast<_float>(rbObject["DynamicFriction"].get_double().value_unsafe());
	m_tRigidBodyDesc.fRestitution = static_cast<_float>(rbObject["Restitution"].get_double().value_unsafe());
	m_tRigidBodyDesc.bLockRotX = rbObject["LockRotX"].get_bool().value_unsafe();
	m_tRigidBodyDesc.bLockRotY = rbObject["LockRotY"].get_bool().value_unsafe();
	m_tRigidBodyDesc.bLockRotZ = rbObject["LockRotZ"].get_bool().value_unsafe();
	m_tRigidBodyDesc.bKinematicTarget = rbObject["KinematicTarget"].get_bool().value_unsafe();
	if (rbObject["IsConvex"].get_bool().error() == simdjson::SUCCESS) // Check if the field exists
	{
		m_tRigidBodyDesc.bIsConvex = rbObject["IsConvex"].get_bool().value_unsafe();
	}


	// 모델 컴포넌트 정보 복원
	if (m_pOwner && rbObject["ModelComponentName"].get_string().error() == simdjson::SUCCESS)
	{
		std::string_view modelComponentNameUtf8 = rbObject["ModelComponentName"].get_string().value_unsafe();
		wstring modelComponentName = Utf8ToWide(std::string(modelComponentNameUtf8));

		// 저장된 이름으로 모델 컴포넌트 가져오기
		CComponent* pModelComponent = m_pOwner->Get_Component(modelComponentName.c_str());
		if (pModelComponent != nullptr)
		{
			// 모델 컴포넌트이면 pModelForMesh에 설정
			m_tRigidBodyDesc.pModelForMesh = static_cast<CModel*>(pModelComponent);
		}
	}

	// Shapes 역직렬화
	m_tRigidBodyDesc.vShapeDescs.clear();
	simdjson::ondemand::array shapesJsonArray;
	if (rbObject["Shapes"].get_array().get(shapesJsonArray) == simdjson::SUCCESS)
	{
		for (auto shapeValue : shapesJsonArray)
		{
			simdjson::ondemand::object shapeObject;
			if (shapeValue.get_object().get(shapeObject) == simdjson::SUCCESS)
			{
				SHAPE_DESC shapeDesc;
				shapeDesc.eShapeType = static_cast<EShapeType>(int(shapeObject["ShapeType"].get_int64().value_unsafe()));

				simdjson::ondemand::array shapeSizeArray;
				if (shapeObject["ShapeSize"].get_array().get(shapeSizeArray) == simdjson::SUCCESS && shapeSizeArray.count_elements() == 3)
				{
					auto iter = shapeSizeArray.begin();
					shapeDesc.vShapeSize.x = static_cast<_float>((*iter).get_double().value_unsafe());
					++iter;
					shapeDesc.vShapeSize.y = static_cast<_float>((*iter).get_double().value_unsafe());
					++iter;
					shapeDesc.vShapeSize.z = static_cast<_float>((*iter).get_double().value_unsafe());
				}

				simdjson::ondemand::array shapeLocalPosArray;
				if (shapeObject["LocalPosition"].get_array().get(shapeLocalPosArray) == simdjson::SUCCESS && shapeLocalPosArray.count_elements() == 3)
				{
					auto iter = shapeLocalPosArray.begin();
					shapeDesc.vLocalPosition.x = static_cast<_float>((*iter).get_double().value_unsafe());
					++iter;
					shapeDesc.vLocalPosition.y = static_cast<_float>((*iter).get_double().value_unsafe());
					++iter;
					shapeDesc.vLocalPosition.z = static_cast<_float>((*iter).get_double().value_unsafe());
				}

				simdjson::ondemand::array shapeRotQuatArray;
				if (shapeObject["RotationQuat"].get_array().get(shapeRotQuatArray) == simdjson::SUCCESS && shapeRotQuatArray.count_elements() == 4)
				{
					auto iter = shapeRotQuatArray.begin();
					shapeDesc.qRotation.x = static_cast<_float>((*iter).get_double().value_unsafe());
					++iter;
					shapeDesc.qRotation.y = static_cast<_float>((*iter).get_double().value_unsafe());
					++iter;
					shapeDesc.qRotation.z = static_cast<_float>((*iter).get_double().value_unsafe());
					++iter;
					shapeDesc.qRotation.w = static_cast<_float>((*iter).get_double().value_unsafe());
				}

				if (shapeObject["EulerRotationX"].get_double().error() == simdjson::SUCCESS)
					shapeDesc.fCurRotationX = static_cast<_float>(shapeObject["EulerRotationX"].get_double().value_unsafe());
				if (shapeObject["EulerRotationY"].get_double().error() == simdjson::SUCCESS)
					shapeDesc.fCurRotationY = static_cast<_float>(shapeObject["EulerRotationY"].get_double().value_unsafe());
				if (shapeObject["EulerRotationZ"].get_double().error() == simdjson::SUCCESS)
					shapeDesc.fCurRotationZ = static_cast<_float>(shapeObject["EulerRotationZ"].get_double().value_unsafe());

				if (shapeObject["IsQueryShape"].get_bool().error() == simdjson::SUCCESS)
					shapeDesc.bIsQueryShape = shapeObject["IsQueryShape"].get_bool().value_unsafe();
				if (shapeObject["IsDoubleSided"].get_bool().error() == simdjson::SUCCESS)
					shapeDesc.bIsDoubleSided = shapeObject["IsDoubleSided"].get_bool().value_unsafe();

				if (shapeObject["FilterData0"].get_uint64().error() == simdjson::SUCCESS)
					shapeDesc.filterData0 = static_cast<physx::PxU32>(shapeObject["FilterData0"].get_uint64().value_unsafe());
				if (shapeObject["FilterData1"].get_uint64().error() == simdjson::SUCCESS)
					shapeDesc.filterData1 = static_cast<physx::PxU32>(shapeObject["FilterData1"].get_uint64().value_unsafe());
				if (shapeObject["FilterData2"].get_uint64().error() == simdjson::SUCCESS)
					shapeDesc.filterData2 = static_cast<physx::PxU32>(shapeObject["FilterData2"].get_uint64().value_unsafe());
				if (shapeObject["FilterData3"].get_uint64().error() == simdjson::SUCCESS)
					shapeDesc.filterData3 = static_cast<physx::PxU32>(shapeObject["FilterData3"].get_uint64().value_unsafe());

				// shapeDesc.pMesh 역직렬화 로직 (예: 메시 이름/경로 기반으로 리소스 매니저에서 찾아 할당)
				// 이 부분은 프로젝트의 리소스 관리 방식에 따라 구현해야 합니다.
				if (shapeObject["MeshIndex"].get_uint64().error() == simdjson::SUCCESS) {
					_int meshIndex = static_cast<_int>(shapeObject["MeshIndex"].get_int64().value_unsafe());
					if (meshIndex != -1)
					{
						shapeDesc.pMesh = m_tRigidBodyDesc.pModelForMesh->Get_Meshes()[meshIndex];
						shapeDesc.iMeshIndex = (_uint)meshIndex;
					}
				}
				m_tRigidBodyDesc.vShapeDescs.push_back(shapeDesc);
			}
		}
	}

	return S_OK;
}
#pragma endregion

#pragma region Loops
void CRigidBody::Priority_Update(_float fTimeDelta)
{
	if (!m_bActive || !m_bLateInitialized || !m_pActor)
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
	if (!m_bActive || !m_bLateInitialized || !m_pActor)
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
		// Body Type
		const char* bodyTypes[] = { "Static", "Dynamic", "Kinematic" };
		int currentBodyType = static_cast<int>(m_tRigidBodyDesc.eBodyType);
		if (ImGui::Combo("Body Type", &currentBodyType, bodyTypes, IM_ARRAYSIZE(bodyTypes)))
		{
			Set_Body_Type(static_cast<EBodyType>(currentBodyType));
		}

		if (ImGui::DragFloat3("Local Position", &m_tRigidBodyDesc.vLocalPosition.x, 0.1f))
		{
			Release_Actor();
			Create_Actor();
		}

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

		// 쉐이프 목록 관리
		if (ImGui::TreeNode("Shapes"))
		{
			// 새 쉐이프 추가 버튼
			if (ImGui::Button("Add Shape"))
			{
				SHAPE_DESC newShape;
				newShape.eShapeType = EShapeType::BOX;
				newShape.vShapeSize = _float3(0.5f, 0.5f, 0.5f);
				m_tRigidBodyDesc.vShapeDescs.push_back(newShape);

				// 쉐이프 추가 시 즉시 액터 재생성
				Release_Actor();
				Create_Actor();
			}
			const char* shapeTypes[] = { "Box", "Sphere", "Capsule", "Convex Mesh", "Triangle Mesh" };
			// 기존 쉐이프 편집
			for (size_t i = 0; i < m_tRigidBodyDesc.vShapeDescs.size(); ++i)
			{
				ImGui::PushID(static_cast<int>(i));

				bool shapeModified = false;
				bool filterDataModified = false;
				SHAPE_DESC& shape = m_tRigidBodyDesc.vShapeDescs[i];

				// 쉐이프 헤더 (삭제 버튼 포함)
				char shapeName[64];
				sprintf_s(shapeName, "Shape %zu: %s", i, shapeTypes[static_cast<int>(shape.eShapeType)]);

				bool shapeOpen = ImGui::TreeNode(shapeName);
				ImGui::SameLine();

				if (ImGui::Button("X"))
				{
					m_tRigidBodyDesc.vShapeDescs.erase(m_tRigidBodyDesc.vShapeDescs.begin() + i);
					Release_Actor();
					Create_Actor();
					ImGui::PopID();
					continue;
				}

				if (shapeOpen)
				{
					// 쉐이프 타입
					int currentShapeType = static_cast<int>(shape.eShapeType);
					if (ImGui::Combo("Shape Type", &currentShapeType, shapeTypes, IM_ARRAYSIZE(shapeTypes)))
					{
						shape.eShapeType = static_cast<EShapeType>(currentShapeType);
						shapeModified = true;
					}

					// 쉐이프 크기
					if (ImGui::DragFloat3("Size", &shape.vShapeSize.x, 0.1f, 0.01f, 100.0f))
					{
						shapeModified = true;
					}

					// 쉐이프 로컬 위치
					if (ImGui::DragFloat3("Local Position", &shape.vLocalPosition.x, 0.1f))
					{
						shapeModified = true;
					}

					// 쉐이프 로컬 회전
					_float3 eulerAnglesDegrees = {
						XMConvertToDegrees(shape.fCurRotationX),
						XMConvertToDegrees(shape.fCurRotationY),
						XMConvertToDegrees(shape.fCurRotationZ)
					};

					if (ImGui::DragFloat3(u8"Rotation (Euler °)", &eulerAnglesDegrees.x, 1.0f, -360.f, 360.f))
					{
						shape.fCurRotationX = XMConvertToRadians(eulerAnglesDegrees.x);
						shape.fCurRotationY = XMConvertToRadians(eulerAnglesDegrees.y);
						shape.fCurRotationZ = XMConvertToRadians(eulerAnglesDegrees.z);

						XMVECTOR newQuaternion = XMQuaternionRotationRollPitchYaw(
							shape.fCurRotationX,  // Pitch
							shape.fCurRotationY,  // Yaw
							shape.fCurRotationZ   // Roll
						);
						XMStoreFloat4(&shape.qRotation, XMQuaternionNormalize(newQuaternion));

						shapeModified = true;
					}

					// Scene Query Shape Flag
					if (ImGui::Checkbox("Is Query Shape", &shape.bIsQueryShape)) shapeModified = true; // Modifying this flag also requires actor/shape recreation

					// Mesh specific options (if applicable)
					//if (shape.eShapeType == EShapeType::CONVEX_MESH || shape.eShapeType == EShapeType::TRIANGLE_MESH)
					//{
					//	ImGui::InputInt("Mesh Index", (int*)&shapeDesc.uiMeshIndex); // Assuming uiMeshIndex is _uint
					//	// Potentially add a combo or drag drop for pModelForMesh if not set globally for the RigidBody
					//	if (ImGui::Checkbox("Force Convex (for Triangle Mesh)", &shape.bIsConvexForShape)) shapeModified = true;
					//}

					// Simulation Filter Data
					if (ImGui::TreeNode("Filter Data"))
					{
						// Make temporary non-const variables for ImGui::InputInt
						_int fd0 = static_cast<int>(shape.filterData0);
						_int fd1 = static_cast<int>(shape.filterData1);
						_int fd2 = static_cast<int>(shape.filterData2);
						_int fd3 = static_cast<int>(shape.filterData3);

						if (ImGui::InputInt("Word0", &fd0)) { shape.filterData0 = static_cast<physx::PxU32>(fd0); filterDataModified = true; }
						if (ImGui::InputInt("Word1", &fd1)) { shape.filterData1 = static_cast<physx::PxU32>(fd1); filterDataModified = true; }
						if (ImGui::InputInt("Word2", &fd2)) { shape.filterData2 = static_cast<physx::PxU32>(fd2); filterDataModified = true; }
						if (ImGui::InputInt("Word3", &fd3)) { shape.filterData3 = static_cast<physx::PxU32>(fd3); filterDataModified = true; }

						ImGui::TreePop();
					}
					ImGui::TreePop();
				}


				if (filterDataModified)
				{
					if (m_pActor && i < m_pShapes.size() && m_pShapes[i])
					{
						physx::PxShape* pShape = m_pShapes[i];
						physx::PxFilterData newFilterData(shape.filterData0, shape.filterData1, shape.filterData2, shape.filterData3);
						pShape->setSimulationFilterData(newFilterData);
						pShape->setQueryFilterData(newFilterData); // Also update query filter data

						// Important: Re-filter the actor in the scene to apply changes
						if (m_pActor->getScene()) {
							m_pActor->getScene()->resetFiltering(*m_pActor);
						}
					}
				}

				// 수정 사항이 있으면 액터 재생성
				if (shapeModified)
				{
					Release_Actor();
					Create_Actor();
				}

				ImGui::PopID();
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

		if (ImGui::TreeNode("Spline Collider Generation"))
		{
			static _float splineColliderRadius = 0.5f;
			ImGui::DragFloat("Collider Radius", &splineColliderRadius, 0.01f, 0.01f, 10.0f);

			if (m_pOwner)
			{
				// 소유자 오브젝트에서 스플라인 컴포넌트 찾기
				CSpline_Component* pSpline = static_cast<CSpline_Component*>(m_pOwner->Get_Component(TEXT("Com_Spline")));

				if (pSpline)
				{
					if (ImGui::Button("Generate Colliders From Spline"))
					{
						Create_Colliders_Along_Spline(pSpline, splineColliderRadius);
					}
				}
				else
				{
					ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "No Spline Component Found");
				}
			}

			ImGui::TreePop();
		}
	}
	ImGui::PopID();
	return S_OK;
}

HRESULT CRigidBody::Add_Shape(const SHAPE_DESC& shapeDesc)
{
	m_tRigidBodyDesc.vShapeDescs.push_back(shapeDesc);

	if (m_bLateInitialized)
	{
		Release_Actor();
		return Create_Actor();
	}

	return S_OK;
}
HRESULT CRigidBody::Remove_Shape(_uint iIndex)
{
	if (iIndex >= m_tRigidBodyDesc.vShapeDescs.size())
		return E_FAIL;

	m_tRigidBodyDesc.vShapeDescs.erase(m_tRigidBodyDesc.vShapeDescs.begin() + iIndex);

	if (m_bLateInitialized)
	{
		Release_Actor();
		return Create_Actor();
	}

	return S_OK;
}
CRigidBody::SHAPE_DESC* CRigidBody::Get_Shape(_uint iIndex)
{
	if (iIndex >= m_tRigidBodyDesc.vShapeDescs.size())
        return nullptr;
    
    return &m_tRigidBodyDesc.vShapeDescs[iIndex];
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
			
			PxVec3 velocityChange = ToPxVec3(vVelocity) - pBody->getLinearVelocity();
			velocityChange.y = pBody->getLinearVelocity().y;
			//pBody->setlin
			pBody->addForce(velocityChange, PxForceMode::eVELOCITY_CHANGE);
			//pBody->setLinearVelocity(ToPxVec3(vVelocity));
			//pBody->set
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
			m_bIsTransformDirty = true;
		}
	}
}

_bool CRigidBody::SweepMove(const _float3& vMove, _float3& outHitNormal, _float& outHitDist, _float fInflation)
{
	PxSweepBuffer hit;
	_float moveDistance = XMVector3Length(XMLoadFloat3(&vMove)).m128_f32[0];
	_float3 moveDir; 
	XMStoreFloat3(&moveDir, XMVector3Normalize(XMLoadFloat3(&vMove)));
	if (moveDistance < 1e-6f) // 0에 가까우면 sweep 호출하지 않음
		return false;

	const PxHitFlags hitFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL; //| PxHitFlag::ePRECISE_SWEEP;
	PxTransform worldShapePose = m_pActor->getGlobalPose() * m_pShapes[0]->getLocalPose();
	_bool isHit = m_pGameInstance->Get_PhysicsScene()->sweep(m_pShapes[0]->getGeometry(), worldShapePose, ToPxVec3(moveDir), moveDistance, hit, hitFlags, PxQueryFilterData(), nullptr, nullptr, fInflation);
	if (isHit)
	{
		const PxSweepHit& sweepHit = hit.block;
		outHitNormal = ToFloat3(sweepHit.normal);
		outHitDist = sweepHit.distance; // 충돌 거리를 반환합니다.
	}
	else
	{
		outHitDist = moveDistance; // 충돌이 없으면 최대 이동 거리를 반환합니다.
	}
	return isHit;
}

_bool CRigidBody::ComputeDepenetration(_float3& outDepenVector)
{
	if (!m_pActor || m_pShapes.empty())
		return false;

	const PxGeometry& geometry = m_pShapes[0]->getGeometry();
	const PxTransform pose = m_pActor->getGlobalPose() * m_pShapes[0]->getLocalPose();

	const PxU32 maxOverlaps = 16;
	PxOverlapHit overlapHits[maxOverlaps];
	PxOverlapBuffer hitBuffer(overlapHits, maxOverlaps);

	PxQueryFilterData filterData;
	filterData.flags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC; // 스태틱과 다이나믹 오브젝트 모두와 겹침을 확인

	if (m_pGameInstance->Get_PhysicsScene()->overlap(geometry, pose, hitBuffer, filterData))
	{
		PxVec3 totalDepenetration(0.f);
		bool resolved = false;

		for (PxU32 i = 0; i < hitBuffer.getNbTouches(); ++i)
		{
			const PxOverlapHit& overlapHit = hitBuffer.getTouch(i);

			// 자기 자신과의 겹침은 무시
			if (overlapHit.actor == m_pActor)
				continue;

			PxVec3 direction;
			PxF32 depth;

			const PxGeometry& otherGeom = overlapHit.shape->getGeometry();
			const PxTransform otherPose = overlapHit.actor->getGlobalPose() * overlapHit.shape->getLocalPose();


			// 겹침 방향과 깊이를 계산
			if (physx::PxGeometryQuery::computePenetration(
				direction, depth,
				geometry, pose,
				otherGeom, otherPose))
			{
				// 모든 겹침에 대한 해소 벡터를 누적
				totalDepenetration += direction * depth;
				resolved = true;
			}
		}

		if (resolved)
		{
			outDepenVector = ToFloat3(totalDepenetration);
			return true;
		}
	}

	return false;
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
	m_tRigidBodyDesc.fStaticFriction = fStaticFriction;
	m_tRigidBodyDesc.fDynamicFriction = fDynamicFriction;
	m_tRigidBodyDesc.fRestitution = fRestitution;

	// 모든 재질에 적용
	for (auto& pMaterial : m_Materials)
	{
		if (pMaterial)
		{
			pMaterial->setStaticFriction(fStaticFriction);
			pMaterial->setDynamicFriction(fDynamicFriction);
			pMaterial->setRestitution(fRestitution);
		}
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

	// 1. Actor 생성 및 위치 설정 준비
	_matrix cTransformWorldMatrix = pTransform->Get_WorldMatrix();

	_float3 rbLocalPos = m_tRigidBodyDesc.vLocalPosition;
	XMVECTOR rbLocalRotQuatVec = XMQuaternionRotationRollPitchYaw(
		m_tRigidBodyDesc.fCurRotationX, // Pitch
		m_tRigidBodyDesc.fCurRotationY, // Yaw
		m_tRigidBodyDesc.fCurRotationZ  // Roll
	);
	rbLocalRotQuatVec = XMQuaternionNormalize(rbLocalRotQuatVec);
	XMStoreFloat4(&m_tRigidBodyDesc.qRotation, rbLocalRotQuatVec);

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
			static_cast<physx::PxRigidDynamic*>(m_pActor)->setRigidBodyFlag(physx::PxRigidBodyFlag::eFORCE_KINE_KINE_NOTIFICATIONS, true);
			static_cast<physx::PxRigidDynamic*>(m_pActor)->setRigidBodyFlag(physx::PxRigidBodyFlag::eFORCE_STATIC_KINE_NOTIFICATIONS, true);
		}
		break;
	default:
		return E_FAIL; // 지원하지 않는 바디 타입
	}

	

	// 2. Shape Geometry 생성
	PxGeometry* pGeometry = nullptr;
	PxTriangleMesh* triangleMesh = nullptr; // Triangle Mesh 리소스
	PxConvexMesh* convexMesh = nullptr;     // Convex Mesh 리소스

	if (m_tRigidBodyDesc.vShapeDescs.empty())
	{
		// 기존 단일 쉐이프 정보를 vShapeDescs에 추가
		SHAPE_DESC singleShapeDesc;
		singleShapeDesc.vShapeSize = m_tRigidBodyDesc.vActorSize;
		singleShapeDesc.vLocalPosition = _float3(0.f, 0.f, 0.f); // 액터 위치에 직접 생성
		singleShapeDesc.qRotation = m_tRigidBodyDesc.qRotation;
		singleShapeDesc.fCurRotationX = m_tRigidBodyDesc.fCurRotationX;
		singleShapeDesc.fCurRotationY = m_tRigidBodyDesc.fCurRotationY;
		singleShapeDesc.fCurRotationZ = m_tRigidBodyDesc.fCurRotationZ;

		m_tRigidBodyDesc.vShapeDescs.push_back(singleShapeDesc);
	}

	for (const auto& shapeDesc : m_tRigidBodyDesc.vShapeDescs)
	{

		physx::PxMaterial* pMaterial = pPhysics->createMaterial(
			m_tRigidBodyDesc.fStaticFriction,
			m_tRigidBodyDesc.fDynamicFriction,
			m_tRigidBodyDesc.fRestitution);

		if (!pMaterial)
			continue;

		XMVECTOR shapeLocalRotQuatVec = XMQuaternionRotationRollPitchYaw(
			shapeDesc.fCurRotationX, // Pitch
			shapeDesc.fCurRotationY, // Yaw
			shapeDesc.fCurRotationZ  // Roll
		);

		// 쉐이프 지오메트리 생성
		PxGeometry* pGeometry = nullptr;

		switch (shapeDesc.eShapeType)
		{
		case EShapeType::BOX:
			pGeometry = new physx::PxBoxGeometry(shapeDesc.vShapeSize.x, shapeDesc.vShapeSize.y, shapeDesc.vShapeSize.z);
			break;
		case EShapeType::SPHERE:
			pGeometry = new physx::PxSphereGeometry(shapeDesc.vShapeSize.x); // radius
			break;
		case EShapeType::CAPSULE:
			pGeometry = new physx::PxCapsuleGeometry(shapeDesc.vShapeSize.x, shapeDesc.vShapeSize.y); // radius, halfHeight
			break;
		case EShapeType::TRIANGLE_MESH:
		{	
			CMesh* pMesh = shapeDesc.pMesh;
			const _float3* vertexDataPtr = pMesh->Get_VertexPositions();
			vector<_uint> tempIndices;
			const void* indicesPtr = nullptr;
			const _uint numVertices = pMesh->Get_NumVertices();
			const _uint numIndices = pMesh->Get_NumIndices();
			

			D3D11_BUFFER_DESC stagingIBDesc;
			stagingIBDesc.ByteWidth = numIndices * sizeof(_uint);
			stagingIBDesc.Usage = D3D11_USAGE_STAGING;
			stagingIBDesc.BindFlags = 0;
			stagingIBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			stagingIBDesc.StructureByteStride = sizeof(_uint);
			stagingIBDesc.MiscFlags = 0;

			ID3D11Buffer* pStagingIB = nullptr;
			if (SUCCEEDED(m_pDevice->CreateBuffer(&stagingIBDesc, nullptr, &pStagingIB)))
			{
				m_pContext->CopyResource(pStagingIB, pMesh->Get_IndexBuffer()); // m_pIB를 가져오는 함수 필요

				D3D11_MAPPED_SUBRESOURCE mappedResource;
				if (SUCCEEDED(m_pContext->Map(pStagingIB, 0, D3D11_MAP_READ, 0, &mappedResource)))
				{
					tempIndices.resize(pMesh->Get_NumIndices());
					memcpy(tempIndices.data(), mappedResource.pData, pMesh->Get_NumIndices() * sizeof(_uint)); // Get_IndexStride() 필요
					m_pContext->Unmap(pStagingIB, 0);
					indicesPtr = tempIndices.data();
				}
				m_pContext->Unmap(pStagingIB, 0);
				Safe_Release(pStagingIB);
			}

			PxTriangleMeshDesc meshDesc;
			meshDesc.points.count = numVertices;
			meshDesc.points.stride = sizeof(_float3);
			meshDesc.points.data = vertexDataPtr;

			meshDesc.triangles.count = static_cast<PxU32>(numIndices / 3);
			meshDesc.triangles.stride = 3 * sizeof(PxU32);
			meshDesc.triangles.data = indicesPtr;

			physx::PxDefaultMemoryOutputStream writeBuffer;
			if (!PxCookTriangleMesh(pPhysics->getTolerancesScale(), meshDesc, writeBuffer)) {
				MSG_BOX("CRigidBody::Create_Actor - Failed to cook triangle mesh using PxCookTriangleMesh.");
				pMaterial->release();
				continue;
			}
			physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
			triangleMesh = pPhysics->createTriangleMesh(readBuffer);
			if (!triangleMesh) {
				MSG_BOX("CRigidBody::Create_Actor - Failed to create PxTriangleMesh from cooked data.");
				pMaterial->release();
				continue;
			}

			if (shapeDesc.bIsDoubleSided)
			{
				pGeometry = new physx::PxTriangleMeshGeometry(triangleMesh, PxMeshScale(ToPxVec3(shapeDesc.vShapeSize)), physx::PxMeshGeometryFlag::eDOUBLE_SIDED);
			}
			else
			{
				pGeometry = new physx::PxTriangleMeshGeometry(triangleMesh, PxMeshScale(ToPxVec3(shapeDesc.vShapeSize)));
			}
			
			
			break;
		}
		case EShapeType::CONVEX_MESH:
		{
			if (!m_tRigidBodyDesc.pModelForMesh) // RigidBody.h에서 pModelForMesh 주석 해제 및 값 설정 필요
			{
				MSG_BOX("CRigidBody::Create_Actor - pModelForMesh is null for MESH shape type.");
				pMaterial->release(); 
				m_Materials.pop_back();
				return E_FAIL;
			}

			CModel* pModel = m_tRigidBodyDesc.pModelForMesh;

			if (pModel->Get_NumMeshes() == 0)
			{
				pMaterial->release();
				continue;
			}
			CMesh* pMesh = shapeDesc.pMesh;
			const _float3* vertexDataPtr = pMesh->Get_VertexPositions();
			const _uint numVertices = pMesh->Get_NumVertices();

			PxConvexMeshDesc convexDesc;
			convexDesc.points.count = numVertices;
			convexDesc.points.stride = sizeof(_float3);
			convexDesc.points.data = vertexDataPtr;
			convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eSHIFT_VERTICES;

			PxDefaultMemoryOutputStream writeBuffer;
			PxConvexMeshCookingResult::Enum cookResult;

			if (!PxCookConvexMesh(pPhysics->getTolerancesScale(), convexDesc, writeBuffer, &cookResult))
			{
				MSG_BOX("CRigidBody::Create_Actor - Failed to cook convex mesh.");
				pMaterial->release();
				continue;
			}

			PxDefaultMemoryInputData input(writeBuffer.getData(), writeBuffer.getSize());
			convexMesh = pPhysics->createConvexMesh(input);

			if (!convexMesh)
			{
				MSG_BOX("CRigidBody::Create_Actor - Failed to create PxConvexMesh from cooked data.");
				pMaterial->release();
				continue;
			}

			pGeometry = new PxConvexMeshGeometry(convexMesh);
		}
		break;
		default:
			pMaterial->release();
			m_Materials.pop_back();
			continue;
		}

		if (!pGeometry)
		{
			pMaterial->release();
			m_Materials.pop_back();
			continue;
		}

		// 쉐이프 로컬 변환 설정
		physx::PxTransform shapePose(
			ToPxVec3(shapeDesc.vLocalPosition),
			ToPxQuat(shapeDesc.qRotation));

		// 쉐이프 생성 및 액터에 부착
		physx::PxShape* pShape = pPhysics->createShape(*pGeometry, *pMaterial, true);
		if (pShape)
		{
			pShape->setContactOffset(0.02f);
			pShape->setRestOffset(0.0f);
			physx::PxFilterData filterData(shapeDesc.filterData0, shapeDesc.filterData1, shapeDesc.filterData2, shapeDesc.filterData3);
			pShape->setSimulationFilterData(filterData);
			pShape->setQueryFilterData(filterData); // Often query filter data is the same, or can be configured separately

			pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
			pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, shapeDesc.bIsQueryShape); // Use bIsQueryShape from desc
			pShape->setLocalPose(shapePose);
			m_pActor->attachShape(*pShape);
			m_pShapes.push_back(pShape);
		}

		delete pGeometry; // 지오메트리는 쉐이프 생성 후 필요 없음
	}

	// 쉐이프가 하나도 생성되지 않았다면 실패
	if (m_pShapes.empty())
	{
		Release_Actor();
		return E_FAIL;
	}

	// 5. Actor를 Scene에 추가
	m_pActor->userData = m_pOwner; // 액터의 userData에 CRigidBody 인스턴스 포인터 저장 (콜백 등에서 사용 가능)
	pScene->addActor(*m_pActor);

 	return S_OK;
}

void CRigidBody::Release_Actor()
{
	PxScene* pScene = m_pGameInstance->Get_PhysicsScene();
	if (m_pActor && pScene)
	{
		pScene->removeActor(*m_pActor);
	}

	// 모든 쉐이프 해제
	//for (auto& pShape : m_pShapes)
	//{
	//	if (pShape)
	//	{
	//		/*m_pActor->detachShape(*pShape);
	//		pShape->release();*/
	//	}
	//}
	m_pShapes.clear();
	
	

	// 모든 재질 해제
	for (auto& pMaterial : m_Materials)
	{
		if (pMaterial)
			pMaterial->release();
	}
	m_Materials.clear();

	if (m_pActor)
	{
		m_pActor->release();
		m_pActor = nullptr;
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
		m_bIsTransformDirty = true;
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
	
	if (!m_pActor || !m_pOwner || !m_bIsTransformDirty) return;
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

HRESULT CRigidBody::Create_Colliders_Along_Spline(CSpline_Component* pSplineComponent, _float fRadius)
{
	if (!pSplineComponent)
		return E_FAIL;

	// 스플라인 포인트 가져오기
	vector<_float3> splinePoints = pSplineComponent->GetSplinePoints();

	// 포인트가 너무 적으면 콜라이더를 생성할 수 없음
	if (splinePoints.size() < 2)
		return E_FAIL;

	// 기존 쉐이프 클리어
	m_tRigidBodyDesc.vShapeDescs.clear();

	// 스플라인 포인트 사이에 캡슐 콜라이더 생성
	for (size_t i = 0; i < splinePoints.size() - 1; ++i)
	{
		const _float3& p1 = splinePoints[i];
		const _float3& p2 = splinePoints[i + 1];

		// 두 점 사이의 거리와 방향 계산
		_float3 direction;
		XMStoreFloat3(&direction, XMVectorSubtract(XMLoadFloat3(&p2), XMLoadFloat3(&p1)));

		_float length = XMVectorGetX(XMVector3Length(XMLoadFloat3(&direction)));

		// 너무 짧은 세그먼트는 무시
		if (length < 0.01f)
			continue;

		// 캡슐의 중앙 위치 계산
		_float3 center;
		XMStoreFloat3(&center, XMVectorScale(XMVectorAdd(XMLoadFloat3(&p1), XMLoadFloat3(&p2)), 0.5f));

		// 캡슐의 방향을 위한 회전 계산
		_float3 upVector = _float3(0.0f, 1.0f, 0.0f);
		_vector directionVec = XMVector3Normalize(XMLoadFloat3(&direction));
		_vector upVec = XMLoadFloat3(&upVector);

		// directionVec과 upVec 사이의 회전을 계산
		_vector rotationQuat = XMQuaternionRotationMatrix(
			XMMatrixLookToRH(XMVectorZero(), directionVec, upVec)
		);

		// 캡슐 콜라이더 생성
		SHAPE_DESC capsuleDesc;
		capsuleDesc.eShapeType = EShapeType::CAPSULE;
		capsuleDesc.vShapeSize = _float3(fRadius, length * 0.5f, 0.0f); // x: 반지름, y: 높이의 절반
		capsuleDesc.vLocalPosition = center;
		XMStoreFloat4(&capsuleDesc.qRotation, rotationQuat);

		// 오일러 각도 계산 (나중에 ImGui에서 편집할 수 있도록)
		XMStoreFloat4(&capsuleDesc.qRotation, rotationQuat);
		XMMATRIX rotMatrix = XMMatrixRotationQuaternion(rotationQuat);

		// 행렬에서 오일러 각도 추출
		_float pitch, yaw, roll;
		roll = atan2f(XMVectorGetX(rotMatrix.r[1]), XMVectorGetY(rotMatrix.r[1]));
		pitch = asinf(-XMVectorGetZ(rotMatrix.r[1]));
		yaw = atan2f(XMVectorGetZ(rotMatrix.r[0]), XMVectorGetZ(rotMatrix.r[2]));

		capsuleDesc.fCurRotationX = pitch;
		capsuleDesc.fCurRotationY = yaw;
		capsuleDesc.fCurRotationZ = roll;

		// 이 형태는 일반적인 충돌 검사용
		capsuleDesc.bIsQueryShape = true;

		// 콜라이더 추가
		m_tRigidBodyDesc.vShapeDescs.push_back(capsuleDesc);
	}

	// 리지드바디가 이미 초기화되었다면 액터 재생성
	if (m_bLateInitialized)
	{
		Release_Actor();
		return Create_Actor();
	}

	return S_OK;
}

#pragma endregion