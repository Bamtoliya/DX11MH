#include "Sonic_Movement_State.h"
#include "GameInstance.h"

#include "Sonic.h"
#include "Sonic_Controller.h"

using namespace Editor;

#pragma region Ground
void CSonic_Movement_State::Idle(_float fTimeDelta)
{
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
	status.fMoveSpeed.first = 0.f;
	XMStoreFloat3(&status.vMoveDirection, XMVectorZero());
	pController->Change_State(m_iStateIndex, ENUM_CLASS(CSonic::STATE::IDLE_LOOP));
	return;
}

void CSonic_Movement_State::Walk(_float fTimeDelta)
{
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
	status.fMoveSpeed.first += status.fMoveAccelration * fTimeDelta;
	if (status.fMoveSpeed.first >= status.fMoveSpeed.second)
		status.fMoveSpeed.first = status.fMoveSpeed.second;
	

}

void CSonic_Movement_State::Run(_float fTimeDelta)
{
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
	status.fMoveSpeed.first += status.fMoveAccelration * 2.f * fTimeDelta;
	if (status.fMoveSpeed.first >= status.fMoveSpeed.second)
		status.fMoveSpeed.first = status.fMoveSpeed.second;
	if (status.fCurSlope >= status.fSlopeLimitAngle)
	{
		pController->Change_State(m_iStateIndex, ENUM_CLASS(CSonic::STATE::WALLRUN_U_LOOP), status.fMoveSpeed.first);
	}
	else
	{
		pController->Change_State(m_iStateIndex, ENUM_CLASS(CSonic::STATE::RUN_LOOP), status.fMoveSpeed.first);
	}
}

void CSonic_Movement_State::Boost(_float fTimeDelta)
{
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
	status.fMoveSpeed.first += status.fMoveAccelration * 5.f * fTimeDelta;
	if (status.fMoveSpeed.first >= status.fMaxBoostSpeed)
		status.fMoveSpeed.first = status.fMaxBoostSpeed;
}

void CSonic_Movement_State::Brake(_float fTimeDelta)
{
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
	status.fMoveSpeed.first *= status.fFriction;
	if (status.fMoveSpeed.first <= 0.f)
		status.fMoveSpeed.first = 0.f;
}

void CSonic_Movement_State::Execute(_float fTimeDelta)
{
	if (!m_pOwner) return;

	CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	CRigidBody* pRigidBody = static_cast<CRigidBody*>(m_pOwner->Get_Component(L"Com_RigidBody"));
	if (!pTransform || !pRigidBody) return;

	CSonic_Controller::SONIC_STATUS& status = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"))->Get_Status();

	// === 1. 지면 탐지 및 정렬 (두 번째 코드 블록의 역할) ===
	const float fRayDistance = 2.0f;
	_vector vRayOrigin = pTransform->Get_State(STATE::POSITION) + pTransform->Get_State(STATE::UP) * 0.5f;
	_vector vRayDirection = -pTransform->Get_State(STATE::UP);

	PxRaycastBuffer hitBuffer;
	if (m_pGameInstance->Get_PhysicsScene()->raycast((PxVec3&)vRayOrigin, (PxVec3&)vRayDirection, fRayDistance, hitBuffer))
	{
		_vector vGroundNormal = XMLoadFloat3((_float3*)&hitBuffer.block.normal);

		// 경사각 체크 (이전 답변 내용)
		status.fCurSlope = acosf(XMVectorGetX(XMVector3Dot(XMVectorSet(0, 1, 0, 0), vGroundNormal))) * 180.f / XM_PI;
		if (status.fCurSlope > status.fSlopeLimitAngle)
		{
			cout << "On Slope\n";
			//static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"))->Change_State(CSonic::STATE::JUMP_D);
			//return;
		}

		// 지면에 맞춰 캐릭터 기울이기
		_vector vCurrentUp = pTransform->Get_State(STATE::UP);
		_vector vRotationAxis = XMVector3Cross(vCurrentUp, vGroundNormal);
		float fRotationAngle = acosf(XMVectorGetX(XMVector3Dot(vCurrentUp, vGroundNormal)));
		const float fGroundRotationSpeed = 10.f;
		if (fRotationAngle > 0.001f)
		{
			_matrix matRotation = XMMatrixRotationAxis(vRotationAxis, min(fRotationAngle, fGroundRotationSpeed * fTimeDelta));
			pTransform->Set_State(STATE::RIGHT, XMVector3TransformNormal(pTransform->Get_State(STATE::RIGHT), matRotation));
			pTransform->Set_State(STATE::UP, XMVector3TransformNormal(pTransform->Get_State(STATE::UP), matRotation));
			pTransform->Set_State(STATE::LOOK, XMVector3TransformNormal(pTransform->Get_State(STATE::LOOK), matRotation));
		}

		// === 2. 입력 기반 방향 회전 (수정된 코드) ===
		_vector vMoveDirection = XMLoadFloat3(&status.vMoveDirection);
		if (status.fMoveSpeed.first > 0.f) // 움직임 입력이 있을 때만 회전
		{
			// 목표 방향을 현재 캐릭터의 지면 평면에 투영합니다.
			_vector vProjectedMoveDir = vMoveDirection - XMVector3Dot(vMoveDirection, pTransform->Get_State(STATE::UP)) * pTransform->Get_State(STATE::UP);
			vProjectedMoveDir = XMVector3Normalize(vProjectedMoveDir);

			// 현재 Look 벡터와 목표 Look 벡터 사이를 부드럽게 회전시킵니다.
			_vector vCurrentLook = pTransform->Get_State(STATE::LOOK);

			// 두 벡터 사이의 각도 계산
			float fDot = XMVectorGetX(XMVector3Dot(vCurrentLook, vProjectedMoveDir));
			// acos의 입력값은 [-1, 1] 범위여야 하므로 클램핑
			fDot = max(-1.f, min(1.f, fDot));
			float fRotationAngle = acosf(fDot);

			const float fTurnSpeed = 100.f; // 좌우 회전 속도 (라디안/초)
			if (fRotationAngle > 0.001f)
			{
				_vector vRotationAxis;

				// [수정 2] 180도 회전 문제 처리
				if (fDot < -0.9999f) // 두 벡터가 거의 정반대 방향일 때
				{
					// 회전 축이 모호하므로, 캐릭터의 UP 벡터를 회전 축으로 사용
					vRotationAxis = pTransform->Get_State(STATE::UP);
				}
				else
				{
					// 그 외의 경우, 외적을 통해 회전 축 계산
					vRotationAxis = XMVector3Cross(vCurrentLook, vProjectedMoveDir);
				}

				// 벡터 외적을 사용하여 회전 방향 결정
				if (XMVectorGetX(XMVector3Dot(pTransform->Get_State(STATE::UP), vRotationAxis)) < 0.f)
				{
					fRotationAngle *= -1.f; // 반대 방향이면 회전각을 음수로
				}

				// [수정 1] 이번 프레임에 회전할 각도를 올바르게 제한(clamp)
				float fMaxTurnThisFrame = fTurnSpeed * fTimeDelta;
				float fClampedAngle = fRotationAngle;

				if (fRotationAngle > fMaxTurnThisFrame) {
					fClampedAngle = fMaxTurnThisFrame;
				}
				else if (fRotationAngle < -fMaxTurnThisFrame) {
					fClampedAngle = -fMaxTurnThisFrame;
				}

				// 최종 회전 행렬 생성 및 적용
				_matrix matTurnRotation = XMMatrixRotationAxis(pTransform->Get_State(STATE::UP), fClampedAngle);

				pTransform->Set_State(STATE::RIGHT, XMVector3TransformNormal(pTransform->Get_State(STATE::RIGHT), matTurnRotation));
				pTransform->Set_State(STATE::LOOK, XMVector3TransformNormal(pTransform->Get_State(STATE::LOOK), matTurnRotation));
			}
		}

		switch (m_eMovementState)
		{
		case CSonic_Movement_State::EMovementState::IDLE:	Idle(fTimeDelta); break;
		case CSonic_Movement_State::EMovementState::WALK:	Walk(fTimeDelta); break;
		case CSonic_Movement_State::EMovementState::RUN:	Run(fTimeDelta); break;
		case CSonic_Movement_State::EMovementState::BOOST:	Boost(fTimeDelta); break;
		case CSonic_Movement_State::EMovementState::BRAKE:	Brake(fTimeDelta); break;
		default:
			break;
		}

		// === 3. 최종 이동 처리 ===
		// 최종적으로 결정된 캐릭터의 Look 방향으로 이동합니다.
		_vector vMovement = pTransform->Get_State(STATE::LOOK) * status.fMoveSpeed.first * fTimeDelta;

		// 지면에 '자석처럼 붙어있어야 할' 정확한 목표 위치를 계산합니다.
		_float fCharacterOffset = 1.0f; // 캐릭터의 높이/반경. 캡슐의 반높이에 해당
		_vector vStickyPosition = XMLoadFloat3((_float3*)&hitBuffer.block.position);
		vStickyPosition.m128_f32[3] = 1.f;
		// 먼저, 현재 캐릭터의 위치를 '붙어있어야 할 위치'로 강제 보정합니다.
		// 이렇게 하면 이전 프레임에서 발생했을 수 있는 미세한 오차(공중에 뜨거나 파고드는 현상)가 즉시 수정됩니다.
		pTransform->Set_State(STATE::POSITION, vStickyPosition);

		// 이제, 완벽하게 지면에 붙은 현재 위치에서 이번 프레임의 이동량을 더해줍니다.
		_matrix newTransform = pTransform->Get_WorldMatrix(); // Set_State로 위치가 보정된 최신 월드 행렬을 가져옵니다.
		newTransform.r[3] += vMovement; // 이동량 적용

		pTransform->Set_State(STATE::POSITION, newTransform.r[3]);
	}
	else
	{
		// 지면이 없으면 공중 상태로 전환
		//static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"))->Change_State(CSonic::STATE::JUMP);
	}
}

void CSonic_Movement_State::HandleInput(_float fTimeDelta)
{
	if (!m_pOwner) return;

	CModel* pModel = static_cast<CModel*>(m_pOwner->Get_Component(L"Com_Model"));
	if (nullptr == pModel) return;

	CAnimator* pAnimator = static_cast<CAnimator*>(m_pOwner->Get_Component(L"Com_Animator"));
	if (nullptr == pAnimator) return;
	CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	if (nullptr == pTransform) return;

	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));

	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();

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
	if (KEY_PRESSING(DIK_Q)) { vMovementDirection -= camRightXZ; }
	if (KEY_PRESSING(DIK_E)) { vMovementDirection += camRightXZ; }

	if (KEY_PRESSING(DIK_LSHIFT))
	{
		m_eMovementState = EMovementState::BOOST;
		if (!bIsMoving)
			XMStoreFloat3(&status.vMoveDirection, XMVector4Normalize(pTransform->Get_State(STATE::LOOK)));
		else 
			XMStoreFloat3(&status.vMoveDirection, XMVector3Normalize(vMovementDirection));
		pController->Change_State(m_iStateIndex, ENUM_CLASS(CSonic::STATE::BOOST_LOOP), status.fMoveSpeed.first);
		return;
	}

	if (KEY_DOWN(DIK_SPACE))
	{
		//status.
		//m_tStateParams.bCanJump = false;
		//pController->Change_State(ENUM_CLASS(CSonic::STATE::JUMP_U));
	}

	if (KEY_DOWN(DIK_I))
	{
		//pController->Change_State(ENUM_CLASS(CSonic::STATE::COMBO_KICK01));
		return;
	}
		
	if (bIsMoving)
	{
		XMStoreFloat3(&status.vMoveDirection, XMVector3Normalize(vMovementDirection));
		if (status.fMoveSpeed.first >= 2.5f)
		{
			m_eMovementState = EMovementState::RUN;
			
		}
		else
		{
			m_eMovementState = EMovementState::WALK;
			pController->Change_State(m_iStateIndex, ENUM_CLASS(CSonic::STATE::WALK_LOOP), 1.5f);
		}
	}
	else
	{
		if (status.fMoveSpeed.first >= 2.f)
		{
			m_eMovementState = EMovementState::BRAKE;
			pController->Change_State(m_iStateIndex, ENUM_CLASS(CSonic::STATE::BRAKE_NEUTRAL), 1.0f, false);
		}
		else
		{
			if (pAnimator->Get_AnimationProgress() >= 0.75f || m_eMovementState == EMovementState::WALK)
			{
				m_eMovementState = EMovementState::IDLE;
				pController->Change_State(m_iStateIndex, ENUM_CLASS(CSonic::STATE::IDLE_LOOP));
			}
		}
	}
	

	
}

CSonic_Movement_State* CSonic_Movement_State::Create(CGameObject* pOwner)
{
	CSonic_Movement_State* pInstance = new CSonic_Movement_State(pOwner);
	pInstance->Initialize();
	return pInstance;
}

#pragma endregion