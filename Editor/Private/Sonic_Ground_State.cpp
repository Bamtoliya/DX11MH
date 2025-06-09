#include "Sonic_Ground_State.h"
#include "GameObject.h"
#include "Animator.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Model.h"
#include "Sonic.h"
#include "Sonic_Controller.h"

using namespace Editor;

#pragma region Ground
CSonic_Ground_State* CSonic_Ground_State::Create(CGameObject* pOwner)
{
	CSonic_Ground_State* pInstance = new CSonic_Ground_State(pOwner);
	pInstance->Initialize();
	return pInstance;
}

HRESULT CSonic_Ground_State::Initialize()
{
	return S_OK;
}

void CSonic_Ground_State::Enter()
{

}

void CSonic_Ground_State::Execute(_float fTimeDelta)
{
	if (!m_pOwner) return;

	CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	CRigidBody* pRigidBody = static_cast<CRigidBody*>(m_pOwner->Get_Component(L"Com_RigidBody"));
	if (!pTransform || !pRigidBody) return;

	// === 1. 지면 탐지 및 정렬 (두 번째 코드 블록의 역할) ===
	const float fRayDistance = 2.0f;
	_vector vRayOrigin = pTransform->Get_State(STATE::POSITION) + pTransform->Get_State(STATE::UP) * 0.5f;
	_vector vRayDirection = -pTransform->Get_State(STATE::UP);

	PxRaycastBuffer hitBuffer;
	if (m_pGameInstance->Get_PhysicsScene()->raycast((PxVec3&)vRayOrigin, (PxVec3&)vRayDirection, fRayDistance, hitBuffer))
	{
		_vector vGroundNormal = XMLoadFloat3((_float3*)&hitBuffer.block.normal);

		// 경사각 체크 (이전 답변 내용)
		const float fSlopeLimitAngle = 60.0f;
		/*if (acosf(XMVectorGetX(XMVector3Dot(XMVectorSet(0, 1, 0, 0), vGroundNormal))) * 180.f / XM_PI > fSlopeLimitAngle)
		{
			static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"))->Change_State(CSonic::STATE::JUMP);
			return;
		}*/

		// 지면에 맞춰 캐릭터 기울이기
		_vector vCurrentUp = pTransform->Get_State(STATE::UP);
		_vector vRotationAxis = XMVector3Cross(vCurrentUp, vGroundNormal);
		float fRotationAngle = acosf(XMVectorGetX(XMVector3Dot(vCurrentUp, vGroundNormal)));
		const float fGroundRotationSpeed = 15.f;
		if (fRotationAngle > 0.001f)
		{
			_matrix matRotation = XMMatrixRotationAxis(vRotationAxis, min(fRotationAngle, fGroundRotationSpeed * fTimeDelta));
			pTransform->Set_State(STATE::RIGHT, XMVector3TransformNormal(pTransform->Get_State(STATE::RIGHT), matRotation));
			pTransform->Set_State(STATE::UP, XMVector3TransformNormal(pTransform->Get_State(STATE::UP), matRotation));
			pTransform->Set_State(STATE::LOOK, XMVector3TransformNormal(pTransform->Get_State(STATE::LOOK), matRotation));
		}

		// === 2. 입력 기반 방향 회전 (첫 번째 코드 블록의 역할) ===
		_vector vMoveDirection = XMLoadFloat3(&m_tStateParams.vMoveDirection);
		if (m_tStateParams.fMoveSpeed > 0.f) // 움직임 입력이 있을 때만 회전
		{
			// 목표 방향을 현재 캐릭터의 지면 평면에 투영합니다.
			_vector vProjectedMoveDir = vMoveDirection - XMVector3Dot(vMoveDirection, pTransform->Get_State(STATE::UP)) * pTransform->Get_State(STATE::UP);
			vProjectedMoveDir = XMVector3Normalize(vProjectedMoveDir);
		
			// 현재 Look 벡터와 목표 Look 벡터 사이를 부드럽게 회전시킵니다.
			_vector vCurrentLook = pTransform->Get_State(STATE::LOOK);
			vRotationAxis = XMVector3Cross(vCurrentLook, vProjectedMoveDir);
			fRotationAngle = acosf(XMVectorGetX(XMVector3Dot(vCurrentLook, vProjectedMoveDir)));
			const float fTurnSpeed = 20.f; // 좌우 회전 속도
			if (fRotationAngle > 0.001f)
			{
				_matrix matTurnRotation = XMMatrixRotationAxis(pTransform->Get_State(STATE::UP), fRotationAngle * fTurnSpeed * fTimeDelta);
				// 더 정확한 회전을 위해 아래와 같이 수정할 수 있습니다.
				// 위 로직은 회전 방향을 고려하지 않으므로, 아래 로직이 더 안정적입니다.
				// 벡터 외적(Cross Product)을 사용하여 회전 방향 결정
				_vector cross = XMVector3Cross(vCurrentLook, vProjectedMoveDir);
				// 캐릭터의 UP 벡터와 외적 결과가 같은 방향인지 확인
				if (XMVectorGetX(XMVector3Dot(pTransform->Get_State(STATE::UP), cross)) < 0) {
					fRotationAngle *= -1.f; // 반대 방향이면 회전각을 음수로
				}
		
				//matTurnRotation = XMMatrixRotationAxis(pTransform->Get_State(STATE::UP), min(fRotationAngle, fTurnSpeed * fTimeDelta));
		
				pTransform->Set_State(STATE::RIGHT, XMVector3TransformNormal(pTransform->Get_State(STATE::RIGHT), matTurnRotation));
				pTransform->Set_State(STATE::LOOK, XMVector3TransformNormal(pTransform->Get_State(STATE::LOOK), matTurnRotation));
			}
		}
		//
		//// === 3. 최종 이동 처리 ===
		//// 최종적으로 결정된 캐릭터의 Look 방향으로 이동합니다.
		//_vector vMovement = pTransform->Get_State(STATE::LOOK) * m_tStateParams.fMoveSpeed * fTimeDelta;
		//
		//// 지면에 붙도록 위치 조정 및 최종 이동
		//_vector vTargetPosition = XMLoadFloat3((_float3*)&hitBuffer.block.position) + vGroundNormal * 1.0f;
		//
		//_matrix newTransform = pTransform->Get_WorldMatrix();
		//// 현재 월드 행렬의 위치를 (이동량 + 지면 스냅 위치)로 갱신
		//newTransform.r[3] = pTransform->Get_State(STATE::POSITION) + vMovement;
		//// 지면 스냅을 위한 최종 높이 보정
		//newTransform.r[3] = XMVectorSetY(newTransform.r[3], vTargetPosition.m128_f32[1]);
		//
		//_float4x4 newFloat4x4; XMStoreFloat4x4(&newFloat4x4, newTransform);
		//pRigidBody->Set_Kinematic_Target(newFloat4x4);

		// === 3. 최종 이동 처리 ===
		// 최종적으로 결정된 캐릭터의 Look 방향으로 이동합니다.
		_vector vMovement = pTransform->Get_State(STATE::LOOK) * m_tStateParams.fMoveSpeed * fTimeDelta;

		// 지면에 '자석처럼 붙어있어야 할' 정확한 목표 위치를 계산합니다.
		_float fCharacterOffset = 1.0f; // 캐릭터의 높이/반경. 캡슐의 반높이에 해당
		_vector vStickyPosition = XMLoadFloat3((_float3*)&hitBuffer.block.position) + vGroundNormal * fCharacterOffset;
		vStickyPosition.m128_f32[3] = 1.f;
		// 먼저, 현재 캐릭터의 위치를 '붙어있어야 할 위치'로 강제 보정합니다.
		// 이렇게 하면 이전 프레임에서 발생했을 수 있는 미세한 오차(공중에 뜨거나 파고드는 현상)가 즉시 수정됩니다.
		pTransform->Set_State(STATE::POSITION, vStickyPosition);

		// 이제, 완벽하게 지면에 붙은 현재 위치에서 이번 프레임의 이동량을 더해줍니다.
		_matrix newTransform = pTransform->Get_WorldMatrix(); // Set_State로 위치가 보정된 최신 월드 행렬을 가져옵니다.
		newTransform.r[3] += vMovement; // 이동량 적용

		// 최종 위치로 Kinematic RigidBody를 이동시킵니다.
		_float4x4 newFloat4x4; XMStoreFloat4x4(&newFloat4x4, newTransform);
		//pRigidBody->Set_Kinematic_Target(newFloat4x4);
	}
	else
	{
		// 지면이 없으면 공중 상태로 전환
		//static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"))->Change_State(CSonic::STATE::JUMP);
	}
}

void CSonic_Ground_State::HandleInput(_float fTimeDelta)
{
	if (!m_pOwner) return;

	CModel* pModel = static_cast<CModel*>(m_pOwner->Get_Component(L"Com_Model"));
	if (nullptr == pModel) return;

	CAnimator* pAnimator = static_cast<CAnimator*>(m_pOwner->Get_Component(L"Com_Animator"));
	if (nullptr == pAnimator) return;
	CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	if (nullptr == pTransform) return;

	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));

	CRigidBody* pRigidBody = static_cast<CRigidBody*>(m_pOwner->Get_Component(L"Com_RigidBody"));
	if (nullptr == pRigidBody) return;

	

	// 1. 카메라의 뷰 행렬 가져오기
	_matrix camWorldMatrix = m_pGameInstance->Get_Transform_MatrixInverse(D3DTS::VIEW);

	// 3. 카메라의 월드 공간 기준 Forward 및 Right 벡터 추출
	_vector camLookWorld = camWorldMatrix.r[2]; // 카메라의 Z축 (바라보는 방향)
	_vector camRightWorld = camWorldMatrix.r[0]; // 카메라의 X축 (오른쪽 방향)

	// 4. 카메라 방향 벡터를 XZ 평면에 투영하고 정규화 (수평 이동을 위해 Y성분 제거)
	_vector camLookXZ = XMVectorSet(XMVectorGetX(camLookWorld), 0.f, XMVectorGetZ(camLookWorld), 0.f);
	camLookXZ = XMVector3Normalize(camLookXZ);

	_vector camRightXZ = XMVectorSet(XMVectorGetX(camRightWorld), 0.f, XMVectorGetZ(camRightWorld), 0.f);
	camRightXZ = XMVector3Normalize(camRightXZ);
	// 또는 camRightXZ = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), camLookXZ); 로 계산 가능

	_vector vMovementDirection = XMVectorZero();
	_bool bIsMoving = false;

	if (KEY_PRESSING(DIK_W)) { vMovementDirection += camLookXZ; bIsMoving = true; }
	if (KEY_PRESSING(DIK_S)) { vMovementDirection -= camLookXZ; bIsMoving = true; }
	if (KEY_PRESSING(DIK_A)) { vMovementDirection -= camRightXZ; bIsMoving = true; }
	if (KEY_PRESSING(DIK_D)) { vMovementDirection += camRightXZ; bIsMoving = true; }

	//if (KEY_PRESSING(DIK_LSHIFT))
	//{
	//	pController->Change_State(ENUM_CLASS(CSonic::STATE::BOOST_LOOP));
	//	return;
	//}

	//if (KEY_DOWN(DIK_SPACE))
	//{
	//	m_tStateParams.bCanJump = false;
	//	//pController->Change_State(ENUM_CLASS(CSonic::STATE::JUMP_U));
	//}

	//if (KEY_DOWN(DIK_I))
	//{
	//	pController->Change_State(ENUM_CLASS(CSonic::STATE::COMBO_KICK01));
	//	return;
	//}
		
		

	if (bIsMoving)
	{
		m_tStateParams.fMoveSpeed = 10.f; // 예시 속도
		XMStoreFloat3(&m_tStateParams.vMoveDirection, XMVector3Normalize(vMovementDirection));
		//pController->Change_State(ENUM_CLASS(CSonic::STATE::RUN_LOOP)); // 애니메이션 상태 등 변경
		pController->Change_State(ENUM_CLASS(CSonic::STATE::WALK_LOOP));
	}
	else
	{
		m_tStateParams.fMoveSpeed = 0.f;
		XMStoreFloat3(&m_tStateParams.vMoveDirection, XMVectorZero());
		pController->Change_State(ENUM_CLASS(CSonic::STATE::IDLE_LOOP));
	}
}

void CSonic_Ground_State::Exit()
{
}

#pragma endregion

#pragma region Idle
CSonic_Idle_State* CSonic_Idle_State::Create(CGameObject* pOwner)
{
	CSonic_Idle_State* pInstance = new CSonic_Idle_State(pOwner);
	pInstance->Initialize();
	return pInstance;
}

HRESULT CSonic_Idle_State::Initialize()
{
	strcpy_s(m_szName, "Sonic_Idle_State");
	return S_OK;
}

void CSonic_Idle_State::Enter()
{
}

void CSonic_Idle_State::Execute(_float fTimeDelta)
{
	__super::Execute(fTimeDelta);
	/*CRigidBody* pRigidBody = static_cast<CRigidBody*>(m_pOwner->Get_Component(L"Com_RigidBody"));
	pRigidBody->Set_Linear_Velocity(_float3(0.f, 0.f, 0.f));*/
}

void CSonic_Idle_State::HandleInput(_float fTimeDelta)
{
	__super::HandleInput(fTimeDelta);
}

void CSonic_Idle_State::Exit()
{
}

#pragma endregion

#pragma region Walk
CSonic_Walk_State* CSonic_Walk_State::Create(CGameObject* pOwner)
{
	CSonic_Walk_State* pInstance = new CSonic_Walk_State(pOwner);
	pInstance->Initialize();
	return pInstance;
}

HRESULT CSonic_Walk_State::Initialize()
{
	strcpy_s(m_szName, "WalkState");
	return S_OK;
}

void CSonic_Walk_State::Enter()
{
}

void CSonic_Walk_State::Execute(_float fTimeDelta)
{
	CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	if (pTransform)
		pTransform->Go_Straight(fTimeDelta);

	CRigidBody* pRigidBody = static_cast<CRigidBody*>(m_pOwner->Get_Component(L"Com_RigidBody"));
	if (nullptr == pRigidBody) return;
	//pRigidBody->Set_Kinematic_Target(pTransform->Get_World4x4());
	
}

void CSonic_Walk_State::HandleInput(_float fTimeDelta)
{
	__super::HandleInput(fTimeDelta);
}

void CSonic_Walk_State::Exit()
{
}
#pragma endregion

#pragma region Run
CSonic_Run_State* CSonic_Run_State::Create(CGameObject* pOwner)
{
	CSonic_Run_State* pInstance = new CSonic_Run_State(pOwner);
	pInstance->Initialize();
	return pInstance;
}


HRESULT CSonic_Run_State::Initialize()
{
	strcpy_s(m_szName, "Sonic_Run_State");
	return S_OK;
}

void CSonic_Run_State::Enter()
{
}

void CSonic_Run_State::Execute(_float fTimeDelta)
{
	__super::Execute(fTimeDelta);
}

void CSonic_Run_State::HandleInput(_float fTimeDelta)
{
	__super::HandleInput(fTimeDelta);
	//if (!m_pOwner) return;

	//CModel* pModel = static_cast<CModel*>(m_pOwner->Get_Component(L"Com_Model"));
	//if (nullptr == pModel) return;

	//CAnimator* pAnimator = static_cast<CAnimator*>(m_pOwner->Get_Component(L"Com_Animator"));
	//if (nullptr == pAnimator) return;
	//CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	//if (nullptr == pTransform) return;

	//CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));

	//CRigidBody* pRigidBody = static_cast<CRigidBody*>(m_pOwner->Get_Component(L"Com_RigidBody"));
	//if (nullptr == pRigidBody) return;

	//// 1. 카메라의 뷰 행렬 가져오기
	//_matrix camWorldMatrix = m_pGameInstance->Get_Transform_MatrixInverse(D3DTS::VIEW);

	//// 3. 카메라의 월드 공간 기준 Forward 및 Right 벡터 추출
	//_vector camLookWorld = camWorldMatrix.r[2]; // 카메라의 Z축 (바라보는 방향)
	//_vector camRightWorld = camWorldMatrix.r[0]; // 카메라의 X축 (오른쪽 방향)

	//// 4. 카메라 방향 벡터를 XZ 평면에 투영하고 정규화 (수평 이동을 위해 Y성분 제거)
	//_vector camLookXZ = XMVectorSet(XMVectorGetX(camLookWorld), 0.f, XMVectorGetZ(camLookWorld), 0.f);
	//camLookXZ = XMVector3Normalize(camLookXZ);

	//_vector camRightXZ = XMVectorSet(XMVectorGetX(camRightWorld), 0.f, XMVectorGetZ(camRightWorld), 0.f);
	//camRightXZ = XMVector3Normalize(camRightXZ);
	//// 또는 camRightXZ = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), camLookXZ); 로 계산 가능

	//_vector vMovementDirection = XMVectorZero();
	//_bool bIsMoving = false;

	//if (KEY_PRESSING(DIK_W))
	//{
	//	vMovementDirection = XMVectorAdd(vMovementDirection, camLookXZ);
	//	bIsMoving = true;
	//}
	//if (KEY_PRESSING(DIK_S))
	//{
	//	vMovementDirection = XMVectorAdd(vMovementDirection, -camLookXZ);
	//	bIsMoving = true;
	//}
	//if (KEY_PRESSING(DIK_A))
	//{
	//	vMovementDirection = XMVectorAdd(vMovementDirection, -camRightXZ);
	//	bIsMoving = true;
	//}
	//if (KEY_PRESSING(DIK_D))
	//{
	//	vMovementDirection = XMVectorAdd(vMovementDirection, camRightXZ);
	//	bIsMoving = true;
	//}

	//if (KEY_PRESSING(DIK_LSHIFT))
	//{
	//	pController->Change_State(ENUM_CLASS(CSonic::STATE::BOOST_LOOP));
	//	return;
	//}


	//if (bIsMoving)
	//{
	//	vMovementDirection = XMVector3Normalize(vMovementDirection); // 최종 이동 방향 정규화

	//	// 캐릭터가 이동할 방향으로 회전 (이 부분은 유지하거나 물리 기반 회전으로 변경 가능)
	//	_float fTargetAngleRadians = atan2f(XMVectorGetX(vMovementDirection), XMVectorGetZ(vMovementDirection));
	//	pTransform->Turn_Quaternion(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTargetAngleRadians);

	//	// 캐릭터 이동 (물리 기반으로 변경)
	//	_float3 vForceDirection;
	//	XMStoreFloat3(&vForceDirection, vMovementDirection);

	//	// 적용할 힘의 크기 (m_tStateParams.fMoveSpeed는 상태별 목표 속도이므로, 가속도로 변환하거나 직접 힘의 크기를 설정)
	//	// 예시: 일정한 가속도 적용
	//	_float fAccelerationMagnitude = 20.f; // 예시 가속도 값, 필요에 따라 조절
	//	//if (pController->Get_CurStateIndex() == ENUM_CLASS(CSonic::STATE::WALK_LOOP))
	//	//	fAccelerationMagnitude = 10.f; // 걷기 상태일 때 가속도
	//	//else if (pController->Get_CurStateIndex() == ENUM_CLASS(CSonic::STATE::RUN_LOOP))
	//	//	fAccelerationMagnitude = 20.f; // 달리기 상태일 때 가속도
	//	//else if (pController->Get_CurStateIndex() == ENUM_CLASS(CSonic::STATE::BOOST_LOOP))
	//	//	fAccelerationMagnitude = 30.f; // 부스트 상태일 때 가속도


	//	_float3 vAppliedForce = { vForceDirection.x * fAccelerationMagnitude, 0.f, vForceDirection.z * fAccelerationMagnitude };
	//	// Y축 힘은 중력이나 점프 로직에서 별도 처리 가정

	//	//pRigidBody->Add_Force(vAppliedForce, physx::PxForceMode::eACCELERATION);


	//	// 상태 전환 로직 (현재 속도에 따라 상태 변경)
	//	// Execute 함수에서 현재 속도를 기반으로 m_tStateParams.fMoveSpeed를 업데이트하고,
	//	// 그 값을 기준으로 상태를 변경하도록 할 수 있습니다.
	//	// 또는 여기서 직접 pRigidBody->Get_Linear_Velocity()를 사용하여 상태 변경
	//	_float3 currentVelocity = pRigidBody->Get_Linear_Velocity();
	//	_float currentSpeed = XMVectorGetX(XMVector3Length(XMLoadFloat3(&currentVelocity)));

	//	//if (pController->Get_CurStateIndex() != ENUM_CLASS(CSonic::STATE::BOOST_LOOP)) // 부스트 중이 아닐 때만 속도 기반 상태 변경
	//	//{
	//	if (currentSpeed >= 10.f) // 이 값은 m_tStateParams.fMoveSpeed와 연동되거나, 별도 임계값 사용
	//		pController->Change_State(ENUM_CLASS(CSonic::STATE::RUN_LOOP));
	//	else
	//		pController->Change_State(ENUM_CLASS(CSonic::STATE::WALK_LOOP));
	//	//}
	//}
	//else
	//{
	//	pController->Change_State(ENUM_CLASS(CSonic::STATE::IDLE_LOOP));
	//}
}

void CSonic_Run_State::Exit()
{
}

#pragma endregion

#pragma region Boost
CSonic_Boost_State* CSonic_Boost_State::Create(CGameObject* pOwner)
{
	CSonic_Boost_State* pInstance = new CSonic_Boost_State(pOwner);
	pInstance->Initialize();
	return pInstance;
}

HRESULT CSonic_Boost_State::Initialize()
{
	strcpy_s(m_szName, "Sonic_Boost_State");
	return S_OK;
}

void CSonic_Boost_State::Enter()
{/*
	CModel* pModel = static_cast<CModel*>(m_pOwner->Get_Component(L"Com_Model"));
	if (nullptr == pModel) return;
	pModel->Set_Animation(ENUM_CLASS(CSonic::STATE::BOOST_LOOP));*/
}

void CSonic_Boost_State::Execute(_float fTimeDelta)
{
	m_tStateParams.fMoveSpeed = 15.f;
}

void CSonic_Boost_State::HandleInput(_float fTimeDelta)
{
	if (!m_pOwner) return;

	CModel* pModel = static_cast<CModel*>(m_pOwner->Get_Component(L"Com_Model"));
	if (nullptr == pModel) return;

	CAnimator* pAnimator = static_cast<CAnimator*>(m_pOwner->Get_Component(L"Com_Animator"));
	if (nullptr == pAnimator) return;
	CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	if (nullptr == pTransform) return;

	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	if (nullptr == pTransform) return;

	// 1. 카메라의 뷰 행렬 가져오기
	_matrix camWorldMatrix = m_pGameInstance->Get_Transform_MatrixInverse(D3DTS::VIEW);

	// 3. 카메라의 월드 공간 기준 Forward 및 Right 벡터 추출
	_vector camLookWorld = camWorldMatrix.r[2]; // 카메라의 Z축 (바라보는 방향)
	_vector camRightWorld = camWorldMatrix.r[0]; // 카메라의 X축 (오른쪽 방향)

	// 4. 카메라 방향 벡터를 XZ 평면에 투영하고 정규화 (수평 이동을 위해 Y성분 제거)
	_vector camLookXZ = XMVectorSet(XMVectorGetX(camLookWorld), 0.f, XMVectorGetZ(camLookWorld), 0.f);
	camLookXZ = XMVector3Normalize(camLookXZ);

	_vector camRightXZ = XMVectorSet(XMVectorGetX(camRightWorld), 0.f, XMVectorGetZ(camRightWorld), 0.f);
	camRightXZ = XMVector3Normalize(camRightXZ);
	// 또는 camRightXZ = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), camLookXZ); 로 계산 가능

	_vector vMovementDirection = XMVectorZero();
	_bool bIsMoving = false;

	if (KEY_PRESSING(DIK_W))
	{
		vMovementDirection = XMVectorAdd(vMovementDirection, camLookXZ);
		bIsMoving = true;
	}
	if (KEY_PRESSING(DIK_S))
	{
		vMovementDirection = XMVectorAdd(vMovementDirection, -camLookXZ);
		bIsMoving = true;
	}
	if (KEY_PRESSING(DIK_A))
	{
		vMovementDirection = XMVectorAdd(vMovementDirection, -camRightXZ);
		bIsMoving = true;
	}
	if (KEY_PRESSING(DIK_D))
	{
		vMovementDirection = XMVectorAdd(vMovementDirection, camRightXZ);
		bIsMoving = true;
	}

	if (KEY_PRESSING(DIK_LSHIFT))
	{
		pController->Change_State(ENUM_CLASS(CSonic::STATE::BOOST_LOOP));
		return;
	}


	if (bIsMoving)
	{
		vMovementDirection = XMVector3Normalize(vMovementDirection); // 최종 이동 방향 정규화

		// 캐릭터가 이동할 방향으로 회전
		_float fTargetAngleRadians = atan2f(XMVectorGetX(vMovementDirection), XMVectorGetZ(vMovementDirection));
		pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTargetAngleRadians);

		// 캐릭터 이동
		//pTransform->Go_Straight(fTimeDelta); // Go_Straight는 현재 Transform의 Look 방향으로 이동
		//pAnimator->Set_CurState(ENUM_CLASS(CSonic::STATE::RUN_LOOP), true);
		//if (m_tStateParams.fMoveSpeed >= 10.f)
		pController->Change_State(ENUM_CLASS(CSonic::STATE::RUN_LOOP));
	}
	else
	{
		pController->Change_State(ENUM_CLASS(CSonic::STATE::BRAKE_NEUTRAL));
	}
}

void CSonic_Boost_State::Exit()
{
}


#pragma endregion

#pragma region Brake
CSonic_Brake_State* CSonic_Brake_State::Create(CGameObject* pOwner)
{
	CSonic_Brake_State* pInstance = new CSonic_Brake_State(pOwner);
	pInstance->Initialize();
	return pInstance;
}


HRESULT CSonic_Brake_State::Initialize()
{
	strcpy_s(m_szName, "Sonic_Break_State");
	m_bLoop = false;
	m_fBlendDuration = 0.5f;
	return S_OK;
}

void CSonic_Brake_State::Enter()
{

}

void CSonic_Brake_State::Execute(_float fTimeDelta)
{
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	CAnimator* pAnimator = static_cast<CAnimator*>(m_pOwner->Get_Component(L"Com_Animator"));

	if (m_tStateParams.fMoveSpeed > 0.f)
		m_tStateParams.fMoveSpeed -= fTimeDelta;

	if (m_tStateParams.fMoveSpeed < 0.f)
		m_tStateParams.fMoveSpeed = 0.f;

	if (pAnimator->Get_AnimationFinished())
		pController->Change_State(ENUM_CLASS(CSonic::STATE::IDLE_LOOP));
}

void CSonic_Brake_State::HandleInput(_float fTimeDelta)
{
	//__super::HandleInput();
}

void CSonic_Brake_State::Exit()
{
}

#pragma endregion