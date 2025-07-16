#include "Sonic_Movement_State.h"
#include "GameInstance.h"

#include "Sonic.h"
#include "Sonic_Controller.h"
#include "Ashura_Movement_State.h"

using namespace Editor;

#pragma region Ground
void CSonic_Movement_State::Idle(_float fTimeDelta)
{
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
	status.fMoveSpeed.first = 0.f;
	XMStoreFloat3(&status.vMoveDirection, XMVectorZero());
	static_cast<CSonic*>(m_pOwner)->Change_ModelIndex(0);
	static_cast<CSonic*>(m_pOwner)->Set_Spin_Collider(false);
	pController->Change_State(ENUM_CLASS(CSonic_Controller::States::MOVE), ENUM_CLASS(CSonic::STATE::IDLE_LOOP));
	return;
}

void CSonic_Movement_State::Walk(_float fTimeDelta)
{
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
	status.fMoveSpeed.first += status.fMoveAccelration * fTimeDelta;
	if (status.fMoveSpeed.first >= status.fMoveSpeed.second)
		status.fMoveSpeed.first = status.fMoveSpeed.second;
	static_cast<CSonic*>(m_pOwner)->Change_ModelIndex(0);
	static_cast<CSonic*>(m_pOwner)->Set_Spin_Collider(false);
	pController->Change_State(ENUM_CLASS(CSonic_Controller::States::MOVE), ENUM_CLASS(CSonic::STATE::WALK_LOOP), 1.5f);
}

void CSonic_Movement_State::Run(_float fTimeDelta)
{
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
	status.fMoveSpeed.first += status.fMoveAccelration * 2.f * fTimeDelta;
	if (status.fMoveSpeed.first >= status.fMoveSpeed.second)
		status.fMoveSpeed.first = status.fMoveSpeed.second;
	static_cast<CSonic*>(m_pOwner)->Change_ModelIndex(0);
	static_cast<CSonic*>(m_pOwner)->Set_Spin_Collider(false);
	if (status.fCurSlope >= status.fSlopeLimitAngle)
	{
		pController->Change_State(ENUM_CLASS(CSonic_Controller::States::MOVE), ENUM_CLASS(CSonic::STATE::WALLRUN_U_LOOP), status.fMoveSpeed.first);
	}
	else
	{
		pController->Change_State(ENUM_CLASS(CSonic_Controller::States::MOVE), ENUM_CLASS(CSonic::STATE::RUN_LOOP), status.fMoveSpeed.first);
	}
}

void CSonic_Movement_State::Dash(_float fTimeDelta)
{
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
	status.fMoveSpeed.first += status.fMoveAccelration * 3.f * fTimeDelta;
	static_cast<CSonic*>(m_pOwner)->Change_ModelIndex(0);
	static_cast<CSonic*>(m_pOwner)->Set_Spin_Collider(false);
	if (status.fMoveSpeed.first > status.fMoveSpeed.second)
	{
		status.fMoveSpeed.first -= status.fMoveAccelration * 5.f * fTimeDelta;
		if (status.fMoveSpeed.first < status.fMoveSpeed.second)
			status.fMoveSpeed.first = status.fMoveSpeed.second;
	}

	pController->Change_State(ENUM_CLASS(CSonic_Controller::States::MOVE), ENUM_CLASS(CSonic::STATE::DASH_LOOP), status.fMoveSpeed.first);
}

void CSonic_Movement_State::Boost(_float fTimeDelta)
{
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
	status.fMoveSpeed.first += status.fMoveAccelration * 5.f * fTimeDelta;
	static_cast<CSonic*>(m_pOwner)->Change_ModelIndex(0);
	static_cast<CSonic*>(m_pOwner)->Set_Spin_Collider(false);
	if (status.fMoveSpeed.first >= status.fMaxBoostSpeed)
		status.fMoveSpeed.first = status.fMaxBoostSpeed;

	if (status.bOnGround)
	{
		pController->Change_State(ENUM_CLASS(CSonic_Controller::States::MOVE), ENUM_CLASS(CSonic::STATE::BOOST_LOOP), status.fMoveSpeed.first, true, 1.f);
	}
	else
	{
		pController->Change_State(ENUM_CLASS(CSonic_Controller::States::MOVE), ENUM_CLASS(CSonic::STATE::BOOST_JUMP_LOOP), status.fMoveSpeed.first, true, 1.f);
	}
	
}

void CSonic_Movement_State::Brake(_float fTimeDelta)
{
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
	status.fMoveSpeed.first *= status.fFriction;
	static_cast<CSonic*>(m_pOwner)->Change_ModelIndex(0);
	static_cast<CSonic*>(m_pOwner)->Set_Spin_Collider(false);
	if (status.fMoveSpeed.first <= 0.f)
		status.fMoveSpeed.first = 0.f;
	if (status.fCurSlope >= 30.f)
	{
		pController->Change_State(ENUM_CLASS(CSonic_Controller::States::MOVE), ENUM_CLASS(CSonic::STATE::BRAKE_SLOPE), 1.0f, false);
	}
	else
	{
		pController->Change_State(ENUM_CLASS(CSonic_Controller::States::MOVE), ENUM_CLASS(CSonic::STATE::BRAKE_NEUTRAL), 1.0f, false);
	}
}

void CSonic_Movement_State::Jump(_float fTimeDelta)
{
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	CAnimator* pAnimator = static_cast<CAnimator*>(m_pOwner->Get_Component(L"Com_Animator"));
	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();

	status.fMoveSpeed.first += status.fMoveAccelration * fTimeDelta;
	if (status.fMoveSpeed.first >= status.fMoveSpeed.second)
		status.fMoveSpeed.first = status.fMoveSpeed.second;

	if (status.fVerticalVelocity.first <= 0.f || pAnimator->Get_AnimationProgress() >= 0.75f)
	{
		static_cast<CSonic*>(m_pOwner)->Change_ModelIndex(1);
		static_cast<CSonic*>(m_pOwner)->Set_Spin_Collider(true);
		pController->Change_State(ENUM_CLASS(CSonic_Controller::States::MOVE), ENUM_CLASS(CSonic::SPINSTATE::SPIN_JUMP), 1.5f, false);
	}
	else
	{
		static_cast<CSonic*>(m_pOwner)->Set_Spin_Collider(false);
		pController->Change_State(ENUM_CLASS(CSonic_Controller::States::MOVE), ENUM_CLASS(CSonic::STATE::JUMP_U), 1.5f, false);
		
	}
}

void CSonic_Movement_State::Rider(_float fTimeDelta)
{
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
	status.fMoveSpeed.first += status.fMoveAccelration * 3.f * fTimeDelta;
	if (status.fMoveSpeed.first > status.fMoveSpeed.second)
	{
		status.fMoveSpeed.first -= status.fMoveAccelration * 5.f * fTimeDelta;
		if (status.fMoveSpeed.first < status.fMoveSpeed.second)
			status.fMoveSpeed.first = status.fMoveSpeed.second;
	}

	static_cast<CSonic*>(m_pOwner)->Change_ModelIndex(0);
	static_cast<CSonic*>(m_pOwner)->Set_Spin_Collider(false);
	pController->Change_State(ENUM_CLASS(CSonic_Controller::States::MOVE), ENUM_CLASS(CSonic::STATE::GRIND_LOOP));
}

void CSonic_Movement_State::Avoid(_float fTimeDelta)
{
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
	CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	status.fMoveSpeed.first += status.fMoveAccelration * 2.f * fTimeDelta;
	if (status.fMoveSpeed.first >= status.fMoveSpeed.second)
		status.fMoveSpeed.first = status.fMoveSpeed.second;
	
	_vector right = pTransform->Get_State(STATE::RIGHT);
	_vector moveDirection = XMLoadFloat3(&status.vMoveDirection);

	float dotProduct = XMVectorGetX(XMVector3Dot(moveDirection, right));

	static_cast<CSonic*>(m_pOwner)->Change_ModelIndex(0);
	static_cast<CSonic*>(m_pOwner)->Set_Spin_Collider(false);
	if (dotProduct > 0.f)
	{
		pController->Change_State(ENUM_CLASS(CSonic_Controller::States::MOVE), ENUM_CLASS(CSonic::STATE::IDLE_QUICKSTEP_R), status.fMoveSpeed.first);
	}
	else
	{
		pController->Change_State(ENUM_CLASS(CSonic_Controller::States::MOVE), ENUM_CLASS(CSonic::STATE::IDLE_QUICKSTEP_L), status.fMoveSpeed.first);
	}
}

void CSonic_Movement_State::QuickStep(_float fTimeDelta)
{
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
	CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	status.fMoveSpeed.first += status.fMoveAccelration * 4.f * fTimeDelta;
	if (status.fMoveSpeed.first >= status.fMaxBoostSpeed)
		status.fMoveSpeed.first = status.fMaxBoostSpeed;

	_vector right = pTransform->Get_State(STATE::RIGHT);

	_vector moveDirection = XMLoadFloat3(&status.vMoveDirection);
	
	float dotProduct = XMVectorGetX(XMVector3Dot(moveDirection, right));

	static_cast<CSonic*>(m_pOwner)->Change_ModelIndex(0);
	static_cast<CSonic*>(m_pOwner)->Set_Spin_Collider(false);

	if (dotProduct > 0.f)
	{
		pController->Change_State(ENUM_CLASS(CSonic_Controller::States::MOVE), ENUM_CLASS(CSonic::STATE::RUN_QUICKSTEP_R), status.fMoveSpeed.first);
	}
	else
	{
		pController->Change_State(ENUM_CLASS(CSonic_Controller::States::MOVE), ENUM_CLASS(CSonic::STATE::RUN_QUICKSTEP_L), status.fMoveSpeed.first);
	}
}

void CSonic_Movement_State::SpringJump(_float fTimeDelta)
{
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	CAnimator* pAnimator = static_cast<CAnimator*>(m_pOwner->Get_Component(L"Com_Animator"));
	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();

	status.fMoveSpeed.first += status.fMoveAccelration * fTimeDelta;
	if (status.fMoveSpeed.first >= status.fMoveSpeed.second)
		status.fMoveSpeed.first = status.fMoveSpeed.second;

	if (status.fVerticalVelocity.first < 0.f || status.vMoveDirection.y < 0.f)
	{
		static_cast<CSonic*>(m_pOwner)->Change_ModelIndex(1);
		static_cast<CSonic*>(m_pOwner)->Set_Spin_Collider(true);
		pController->Change_State(ENUM_CLASS(CSonic_Controller::States::MOVE), ENUM_CLASS(CSonic::SPINSTATE::SPIN_JUMP), 1.5f, false);
	}
	else
	{
		static_cast<CSonic*>(m_pOwner)->Set_Spin_Collider(false);
		pController->Change_State(ENUM_CLASS(CSonic_Controller::States::MOVE), ENUM_CLASS(CSonic::STATE::SPRINGJUMP_LOOP), 1.5f, false);

	}
}

void CSonic_Movement_State::ResolveCollisions(_vector vMovement, _float fTimeDelta)
{
	CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	CRigidBody* pRigidBody = static_cast<CRigidBody*>(m_pOwner->Get_Component(L"Com_RigidBody"));
	CSonic_Controller::SONIC_STATUS& status = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"))->Get_Status();

	const float skinWidth = 0.02f;
	const int maxIterations = 5;
	_vector remainingMovement = vMovement;

	for (int i = 0; i < maxIterations; ++i)
	{
		_float remainingLength = XMVectorGetX(XMVector3Length(remainingMovement));
		if (remainingLength < 1e-6f)
			break;

		_float3 v3Move;
		XMStoreFloat3(&v3Move, remainingMovement);
		_float3 hitNormal_float3;
		_float hitDist;

		if (pRigidBody->SweepMove(v3Move, hitNormal_float3, hitDist, skinWidth))
		{
			_vector vHitNormal = XMLoadFloat3(&hitNormal_float3);
			_vector vMoveDir = XMVector3Normalize(remainingMovement);

			if (!status.bOnGround) // 공중 상태일 때만 수직 속도 처리
			{
				float fVerticalImpact = XMVectorGetX(XMVector3Dot(vHitNormal, pTransform->Get_State(STATE::UP)));
				if (abs(fVerticalImpact) > 0.5f)
				{
					status.fVerticalVelocity.first = 0.f;
				}
			}

			if (hitDist < 0.001f) // 끼임 상태 처리
			{
				const float unstickOffset = 0.002f;
				pTransform->Set_State(STATE::POSITION, pTransform->Get_State(STATE::POSITION) + vHitNormal * unstickOffset);

				float dotProduct = XMVectorGetX(XMVector3Dot(remainingMovement, vHitNormal));
				remainingMovement = remainingMovement - vHitNormal * dotProduct;
			}
			else // 일반 충돌
			{
				pTransform->Set_State(STATE::POSITION, pTransform->Get_State(STATE::POSITION) + vMoveDir * max(0.f, hitDist));

				float remainingAfterImpactDist = remainingLength - hitDist;
				if (remainingAfterImpactDist > 1e-6f)
				{
					_vector vLeftoverMove = vMoveDir * remainingAfterImpactDist;
					float dotProduct = XMVectorGetX(XMVector3Dot(vLeftoverMove, vHitNormal));
					remainingMovement = vLeftoverMove - vHitNormal * dotProduct;
				}
				else
				{
					remainingMovement = XMVectorZero();
				}
			}
		}
		else
		{
			pTransform->Set_State(STATE::POSITION, pTransform->Get_State(STATE::POSITION) + remainingMovement);
			break;
		}
	}
}

void CSonic_Movement_State::Execute(_float fTimeDelta)
{
	if (!m_pOwner) return;
	
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	CRigidBody* pRigidBody = static_cast<CRigidBody*>(m_pOwner->Get_Component(L"Com_RigidBody"));
	if (!pTransform || !pRigidBody) return;

	CSonic_Controller::SONIC_STATUS& status = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"))->Get_Status();

	if (nullptr != pController->Get_CurrentSpline() && !status.bIsOnSpline)
	{
		Begin_Spline();
	}

	if (status.bIsOnSpline)
	{
		Update_SplineMovement(fTimeDelta);
		if (m_eMovementState == EMovementState::RIDER)
			Rider(fTimeDelta);
		return;
	}

	if (status.bIsSpring)
	{
		SpringJump(fTimeDelta);
	}

	// === 1. 지면 탐지 및 정렬 (두 번째 코드 블록의 역할) ===
	const float fRayDistance = 1.0f;
	_vector vRayOrigin = pTransform->Get_State(STATE::POSITION) + pTransform->Get_State(STATE::UP) * 0.5f;
	_vector vRayDirection = -pTransform->Get_State(STATE::UP);

	_vector vGlobalRayDirection = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	_vector vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	PxRaycastBuffer hitBuffer;

	if (m_pGameInstance->Get_PhysicsScene()->raycast((PxVec3&)vRayOrigin, (PxVec3&)vGlobalRayDirection, fRayDistance, hitBuffer) && status.fVerticalVelocity.first <= 0)
	{
		_vector vCurrentUp = pTransform->Get_State(STATE::UP);
		// 캐릭터의 UP 벡터와 월드 UP 벡터의 내적이 0.1보다 작으면 (거의 90도 차이), 누워있는 것으로 간주
		if (XMVectorGetX(XMVector3Dot(vCurrentUp, vWorldUp)) < 0.1f)
		{
			_vector vCurrentLook = pTransform->Get_State(STATE::LOOK);

			// 현재 Look 방향을 수평면에 투영하여 새로운 Look 방향으로 설정
			_vector vNewLook = vCurrentLook - XMVector3Dot(vCurrentLook, vWorldUp) * vWorldUp;
			vNewLook = XMVector3Normalize(vNewLook);

			// 새로운 Right 벡터 계산
			_vector vNewRight = XMVector3Cross(vWorldUp, vNewLook);
			vNewRight = XMVector3Normalize(vNewRight);

			// 변환 적용
			pTransform->Set_State(STATE::RIGHT, vNewRight);
			pTransform->Set_State(STATE::UP, vWorldUp);
			pTransform->Set_State(STATE::LOOK, vNewLook);
		}
	}
	
	if (m_pGameInstance->Get_PhysicsScene()->raycast((PxVec3&)vRayOrigin, (PxVec3&)vRayDirection, fRayDistance, hitBuffer) && status.fVerticalVelocity.first <= 0)
	{
		_vector vGroundNormal = XMLoadFloat3((_float3*)&hitBuffer.block.normal);

		// 경사각 계산 (단순히 정보 제공 목적)
		status.fCurSlope = acosf(XMVectorGetX(XMVector3Dot(XMVectorSet(0, 1, 0, 0), vGroundNormal))) * 180.f / XM_PI;
		if (status.fCurSlope > status.fSlopeLimitAngle)
		{
			cout << "On Slope\n";
			//static_cast<CSonic_Controller*>(m_pO	wner->Get_Component(L"Com_Controller"))->Change_State(CSonic::STATE::JUMP_D);
			//return;
		}

		// 이전에 공중에 있었다면 착지한 것임
		if (!status.bOnGround)
		{
			status.fVerticalVelocity.first = 0.f;
			status.bCanJump = true;
			status.bCanDoubleJump = false;
			status.bIsSpring = false;
			if (status.fMoveSpeed.first > 0.f)
			{
				m_eMovementState = EMovementState::WALK;
			}
			else
			{
				m_eMovementState = EMovementState::IDLE; // IDLE로 초기화, HandleInput에서 이동 상태로 변경
			}
		}
		status.bIsOnSpline = false;
		status.bOnGround = true;

		// === 속도 기반 표면 접착력 계산 ===
		// 속도가 빠를수록 더 가파른 경사로와 루프를 탈 수 있음
		float fMinSpeedForLoop = 5.0f; // 루프를 타기 위한 최소 속도
		float fMaxSlopeAtMinSpeed = 60.0f; // 최소 속도에서 탈 수 있는 최대 경사각
		float fMaxSlopeAtMaxSpeed = 360.0f; // 최대 속도에서 탈 수 있는 최대 경사각

		// 현재 속도에 따른 최대 경사각 계산 (선형 보간)
		float fCurrentMaxSlope = fMaxSlopeAtMinSpeed;
		if (status.fMoveSpeed.first >= fMinSpeedForLoop) {
			float speedRatio = min(1.0f, (status.fMoveSpeed.first - fMinSpeedForLoop) /
				(status.fMaxBoostSpeed - fMinSpeedForLoop));
			fCurrentMaxSlope = fMaxSlopeAtMinSpeed +
				speedRatio * (fMaxSlopeAtMaxSpeed - fMaxSlopeAtMinSpeed);
		}

		// 현재 표면이 타기에 너무 가파르면 미끄러짐 상태로 전환 가능
		bool bTooSteep = status.fCurSlope > fCurrentMaxSlope;

		// 미끄러짐 상태 처리는 여기에 추가 (현재는 생략)

		// === 지면에 맞춰 캐릭터 완전히 정렬 (360도 회전 지원) ===
		// 1. 현재 Look 방향이 지면에 평행하도록 투영
		_vector vCurrentLook = pTransform->Get_State(STATE::LOOK);
		_vector vProjectedLook = vCurrentLook - XMVector3Dot(vCurrentLook, vGroundNormal) * vGroundNormal;
		vProjectedLook = XMVector3Normalize(vProjectedLook);

		// 2. UP 벡터를 지면 법선과 일치시킴
		_vector vNewUp = vGroundNormal;

		// 3. 새 RIGHT 벡터 계산 (새 UP과 새 LOOK의 외적)
		_vector vNewRight = XMVector3Cross(vNewUp, vProjectedLook);
		vNewRight = XMVector3Normalize(vNewRight);

		// 4. LOOK 벡터 재계산 (RIGHT와 UP의 외적으로 정확한 직교성 보장)
		_vector vNewLook = XMVector3Cross(vNewRight, vNewUp);
		vNewLook = XMVector3Normalize(vNewLook);

		// 5. 회전 속도 제한 (너무 급격한 회전 방지)
		const float fMaxRotationRate = 10.0f; // 초당 최대 회전 속도
		float fRotationSpeed = min(fMaxRotationRate * fTimeDelta, 1.0f);

		// 6. 현재 방향에서 목표 방향으로 부드럽게 보간
		_vector vCurrentRight = pTransform->Get_State(STATE::RIGHT);
		_vector vCurrentUp = pTransform->Get_State(STATE::UP);

		_vector vInterpolatedRight = XMVectorLerp(vCurrentRight, vNewRight, fRotationSpeed);
		_vector vInterpolatedUp = XMVectorLerp(vCurrentUp, vNewUp, fRotationSpeed);
		_vector vInterpolatedLook = XMVectorLerp(vCurrentLook, vNewLook, fRotationSpeed);

		// 정규직교화 (orthonormalization)로 벡터 정확도 유지
		vInterpolatedRight = XMVector3Normalize(vInterpolatedRight);
		vInterpolatedUp = XMVector3Normalize(vInterpolatedUp);
		vInterpolatedLook = XMVector3Cross(vInterpolatedRight, vInterpolatedUp);
		vInterpolatedLook = XMVector3Normalize(vInterpolatedLook);
		vInterpolatedRight = XMVector3Cross(vInterpolatedUp, vInterpolatedLook);

		// 변환 행렬 적용
		pTransform->Set_State(STATE::RIGHT, vInterpolatedRight);
		pTransform->Set_State(STATE::UP, vInterpolatedUp);
		pTransform->Set_State(STATE::LOOK, vInterpolatedLook);

		// === 2. 입력 기반 방향 회전 (수정된 코드) ===
		_vector vMoveDirection = XMLoadFloat3(&status.vMoveDirection);
		if (status.fMoveSpeed.first > 0.f) // 움직임 입력이 있을 때만 회전
		{
			// 이동 방향을 현재 표면 평면에 투영
			_vector vProjectedMoveDir = vMoveDirection - XMVector3Dot(vMoveDirection, vInterpolatedUp) * vInterpolatedUp;
			vProjectedMoveDir = XMVector3Normalize(vProjectedMoveDir);

			// 현재 Look 벡터와 투영된 이동 벡터 사이 각도 계산
			float fDot = XMVectorGetX(XMVector3Dot(vInterpolatedLook, vProjectedMoveDir));
			fDot = max(-1.f, min(1.f, fDot));
			float fRotationAngle = acosf(fDot);

			const float fTurnSpeed = 100.f; // 좌우 회전 속도 (라디안/초)
			if (fRotationAngle > 0.001f)
			{
				_vector vRotationAxis;

				// 180도 회전 처리
				if (fDot < -0.9999f)
				{
					vRotationAxis = vInterpolatedUp; // 캐릭터의 UP 벡터를 회전 축으로 사용
				}
				else
				{
					vRotationAxis = XMVector3Cross(vInterpolatedLook, vProjectedMoveDir);
				}

				// 회전 방향 결정
				if (XMVectorGetX(XMVector3Dot(vInterpolatedUp, vRotationAxis)) < 0.f)
				{
					fRotationAngle *= -1.f;
				}

				// 이번 프레임에 회전할 각도 제한
				float fMaxTurnThisFrame = fTurnSpeed * fTimeDelta;
				float fClampedAngle = fRotationAngle;

				if (fRotationAngle > fMaxTurnThisFrame) {
					fClampedAngle = fMaxTurnThisFrame;
				}
				else if (fRotationAngle < -fMaxTurnThisFrame) {
					fClampedAngle = -fMaxTurnThisFrame;
				}

				// 회전 행렬 생성 및 적용
				_matrix matTurnRotation = XMMatrixRotationAxis(vInterpolatedUp, fClampedAngle);

				pTransform->Set_State(STATE::RIGHT, XMVector3TransformNormal(pTransform->Get_State(STATE::RIGHT), matTurnRotation));
				pTransform->Set_State(STATE::LOOK, XMVector3TransformNormal(pTransform->Get_State(STATE::LOOK), matTurnRotation));
			}
		}

		// 상태에 따른 이동 속도 처리
		switch (m_eMovementState)
		{
		case CSonic_Movement_State::EMovementState::IDLE:		Idle(fTimeDelta); break;
		case CSonic_Movement_State::EMovementState::WALK:		Walk(fTimeDelta); break;
		case CSonic_Movement_State::EMovementState::RUN:		Run(fTimeDelta); break;
		case CSonic_Movement_State::EMovementState::DASH:		Dash(fTimeDelta); break;
		case CSonic_Movement_State::EMovementState::BOOST:		Boost(fTimeDelta); break;
		case CSonic_Movement_State::EMovementState::BRAKE:		Brake(fTimeDelta); break;
		case CSonic_Movement_State::EMovementState::JUMP:		Jump(fTimeDelta); break;
		case CSonic_Movement_State::EMovementState::RIDER:		Rider(fTimeDelta); break;
		case CSonic_Movement_State::EMovementState::SPRINGJUMP:	SpringJump(fTimeDelta); break;
		case CSonic_Movement_State::EMovementState::AVOID:		Avoid(fTimeDelta); break;
		case CSonic_Movement_State::EMovementState::QUICKSTEP:	QuickStep(fTimeDelta); break;
		default:
			break;
		}

		// === 3. 최종 이동 처리 ===
		// 최종적으로 결정된 캐릭터의 Look 방향으로 이동합니다.


		// 지면에 '자석처럼 붙어있어야 할' 정확한 목표 위치를 계산합니다.
		_float fCharacterOffset = 1.0f; // 캐릭터의 높이/반경. 캡슐의 반높이에 해당
		_vector vStickyPosition = XMLoadFloat3((_float3*)&hitBuffer.block.position);
		vStickyPosition.m128_f32[3] = 1.f;

		_vector vCurrentPosition = pTransform->Get_State(STATE::POSITION);
		_vector vDiff = XMVectorSubtract(vStickyPosition, vCurrentPosition);
		float fVerticalDistance = XMVectorGetY(vDiff); // 수직 이동 거리

		const float fMaxSnapUpDistance = 0.2f; // 위로 올라탈 수 있는 최대 거리 (예시)
		const float fMaxSnapDownDistance = 1.0f; // 아래로 내려갈 수 있는 최대 거리 (예시)

		if (fVerticalDistance > fMaxSnapUpDistance) // 너무 높이 올라타려고 하면
		{
			// 위치를 보정하지 않거나, 최대치만큼만 올림
			vStickyPosition = vCurrentPosition + XMVectorSet(0.f, fMaxSnapUpDistance, 0.f, 0.f);
		}
		else if (fVerticalDistance < -fMaxSnapDownDistance) // 너무 깊이 내려가려고 하면
		{
			vStickyPosition = vCurrentPosition - XMVectorSet(0.f, fMaxSnapDownDistance, 0.f, 0.f);
		}
		// 먼저, 현재 캐릭터의 위치를 '붙어있어야 할 위치'로 강제 보정합니다.
		// 이렇게 하면 이전 프레임에서 발생했을 수 있는 미세한 오차(공중에 뜨거나 파고드는 현상)가 즉시 수정됩니다.
		pTransform->Set_State(STATE::POSITION, vStickyPosition);

		const float skinWidth = 0.02f;

		_float3 depenetrationVector;
		if (pRigidBody->ComputeDepenetration(depenetrationVector))
		{
			// 겹쳐있다면, 밖으로 밀어내는 벡터를 적용
			const float depenBias = 0.01f;
			_vector depenDir = XMVector3Normalize(XMLoadFloat3(&depenetrationVector));
			_vector depenMag = XMVector3Length(XMLoadFloat3(&depenetrationVector));

			// 겹친 거리 + 약간의 여유만큼 밀어냅니다.
			pTransform->Set_State(STATE::POSITION, pTransform->Get_State(STATE::POSITION) + depenDir * (XMVectorGetX(depenMag) + skinWidth));
			//pTransform->Set_State(STATE::POSITION, pTransform->Get_State(STATE::POSITION) + XMLoadFloat3(&depenetrationVector));
		}

		_vector vMovement;
		if (m_eMovementState == EMovementState::AVOID || m_eMovementState == EMovementState::QUICKSTEP)
		{
			vMovement = XMLoadFloat3(&status.vMoveDirection) * status.fMoveSpeed.first * fTimeDelta;
		}
		else
		{
			vMovement = pTransform->Get_State(STATE::LOOK) * status.fMoveSpeed.first * fTimeDelta;
		}

		// 이제, 완벽하게 지면에 붙은 현재 위치에서 이번 프레임의 이동량을 더해줍니다.
		_matrix newTransform = pTransform->Get_WorldMatrix(); // Set_State로 위치가 보정된 최신 월드 행렬을 가져옵니다.
		newTransform.r[3] += vMovement; // 이동량 적용

		ResolveCollisions(vMovement, fTimeDelta);
		return;
	}
	else
	{
		switch (m_eMovementState)
		{
		case Editor::CSonic_Movement_State::EMovementState::BOOST: Boost(fTimeDelta); break;
		case Editor::CSonic_Movement_State::EMovementState::SPRINGJUMP: SpringJump(fTimeDelta); break;
		case Editor::CSonic_Movement_State::EMovementState::POLESPIN: break;
		case Editor::CSonic_Movement_State::EMovementState::PULLEY: break;
		case Editor::CSonic_Movement_State::EMovementState::RIDER: Rider(fTimeDelta); break;
		default:
			Jump(fTimeDelta);
			break;
		}
		
		status.bIsOnSpline = false;
		status.bOnGround = false;
		const float fGravity = -9.8f;
		status.fVerticalVelocity.first += fGravity * fTimeDelta;

		// 수직 속도를 양수(점프)와 음수(낙하) 모두에 대해 최대 속도로 제한합니다.
		status.fVerticalVelocity.first = max(-status.fVerticalVelocity.second, min(status.fVerticalVelocity.first, status.fVerticalVelocity.second));

		// 공중 이동 벡터 계산
		_vector vAirMovement = XMLoadFloat3(&status.vMoveDirection) * status.fMoveSpeed.first * fTimeDelta;
		_vector vVerticalMovement = XMVectorSet(0.f, 1.0f, 0.f, 0.f) * status.fVerticalVelocity.first * fTimeDelta;

		// 최종 이동 벡터
		_vector vTotalMovement = vAirMovement + vVerticalMovement;

		// Sweep으로 충돌 감지하며 이동
		_float3 v3Move;
		XMStoreFloat3(&v3Move, vTotalMovement);
	  	const float skinWidth = 0.02f;

		_float3 depenetrationVector;
		if (pRigidBody->ComputeDepenetration(depenetrationVector))
		{
			// 겹쳐있다면, 밖으로 밀어내는 벡터를 적용
			const float depenBias = 0.01f;
			_vector depenDir = XMVector3Normalize(XMLoadFloat3(&depenetrationVector));
			_vector depenMag = XMVector3Length(XMLoadFloat3(&depenetrationVector));

			// 겹친 거리 + 약간의 여유만큼 밀어냅니다.
			pTransform->Set_State(STATE::POSITION, pTransform->Get_State(STATE::POSITION) + depenDir * (XMVectorGetX(depenMag) + skinWidth));
			//pTransform->Set_State(STATE::POSITION, pTransform->Get_State(STATE::POSITION) + XMLoadFloat3(&depenetrationVector));
		}

		ResolveCollisions(vTotalMovement, fTimeDelta);
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

	if (status.bIsSpring)
	{
		m_eMovementState = EMovementState::SPRINGJUMP;
	}

	if (status.bIsOnSpline)
	{
		m_eMovementState = EMovementState::RIDER;
		// 레일에서 점프
		if (KEY_DOWN(DIK_SPACE))
		{
			status.bIsOnSpline = false;
			pController->Unregister_Spline();

			m_eMovementState = EMovementState::JUMP;
			status.fVerticalVelocity.first = status.fJumpForce;
			status.bOnGround = false;
			status.bCanJump = false;
			status.bCanDoubleJump = true;
			return; // 입력 처리 완료
		}

		//// 레일에서 퀵스텝 (좌/우 점프)
		//_bool bQuickStep = false;
		//_vector vSideStepDir = XMVectorZero();
		//if (KEY_DOWN(DIK_Q))
		//{
		//	vSideStepDir = -pTransform->Get_State(STATE::RIGHT); // 레일의 바이노멀(오른쪽) 벡터
		//	bQuickStep = true;
		//}
		//else if (KEY_DOWN(DIK_E))
		//{
		//	vSideStepDir = pTransform->Get_State(STATE::RIGHT);
		//	bQuickStep = true;
		//}

		//if (bQuickStep)
		//{
		//	status.bIsOnSpline = false;
		//	pController->Unregister_Spline();

		//	m_eMovementState = EMovementState::JUMP; // 점프 상태 재사용
		//	status.fVerticalVelocity.first = status.fJumpForce * 0.7f; // 일반 점프보다 낮은 점프

		//	// 측면 이동 속도 및 방향 설정
		//	XMStoreFloat3(&status.vMoveDirection, XMVector3Normalize(vSideStepDir));
		//	status.fMoveSpeed.first = 10.f; // 퀵스텝을 위한 고정 속도

		//	status.bOnGround = false;
		//	status.bCanJump = false;
		//	status.bCanDoubleJump = false; // 퀵스텝 후에는 더블 점프 불가

		//	// 퀵스텝 애니메이션 재생 (필요에 따라 애니메이션 인덱스 조정)
		//	pController->Change_State(ENUM_CLASS(CSonic_Controller::States::MOVE), ENUM_CLASS(CSonic::STATE::IDLE_QUICKSTEP_R), 1.5f, false, 0.1f);
		//	return; // 입력 처리 완료
		//}
	}
	else
	{
		CRigidBody* pRigidBody = static_cast<CRigidBody*>(m_pOwner->Get_Component(L"Com_RigidBody"));
		if (nullptr == pRigidBody) return;

		// 카메라의 월드 행렬 가져오기
		_matrix camWorldMatrix = m_pGameInstance->Get_Transform_MatrixInverse(D3DTS::VIEW);

		// 카메라의 월드 공간 기준 Forward 및 Right 벡터 추출
		_vector camLookWorld = camWorldMatrix.r[2]; // 카메라의 Z축 (바라보는 방향)
		_vector camRightWorld = camWorldMatrix.r[0]; // 카메라의 X축 (오른쪽 방향)
		_vector camUpWorld = camWorldMatrix.r[1];    // 카메라의 Y축 (위쪽 방향)

		// 캐릭터의 현재 UP 벡터 가져오기 (표면 법선 방향)
		_vector characterUp = pTransform->Get_State(STATE::UP);

		// 캐릭터 표면의 평면 구성
		// 캐릭터의 UP 벡터에 수직인 벡터들이 이 평면을 형성

		// 카메라 Look 벡터를 캐릭터의 표면 평면에 투영
		_vector projectedCamLook = camLookWorld - XMVector3Dot(camLookWorld, characterUp) * characterUp;
		float projectedLookLength = XMVectorGetX(XMVector3Length(projectedCamLook));

		// 너무 작은 벡터(거의 수직인 경우)는 기본값으로 대체
		if (projectedLookLength < 0.01f) {
			// 캐릭터의 현재 Look 벡터와 카메라의 Right 벡터의 외적으로 대체 방향 계산
			projectedCamLook = XMVector3Cross(characterUp, camRightWorld);
			if (XMVectorGetX(XMVector3Length(projectedCamLook)) < 0.01f) {
				// 비상 대안으로 월드 축 사용
				projectedCamLook = XMVector3Cross(characterUp, XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f));
			}
		}
		projectedCamLook = XMVector3Normalize(projectedCamLook);

		// 카메라 Right 벡터도 표면 평면에 투영
		_vector projectedCamRight = XMVector3Cross(characterUp, projectedCamLook);
		projectedCamRight = XMVector3Normalize(projectedCamRight);

		// 입력에 따른 이동 방향 계산 (이제 표면 평면에 제대로 투영된 벡터 사용)
		_vector vMovementDirection = XMVectorZero();
		_bool bIsMoving = false;

		if (KEY_PRESSING(DIK_W)) { vMovementDirection += projectedCamLook; bIsMoving = true; }
		if (KEY_PRESSING(DIK_S)) { vMovementDirection -= projectedCamLook; bIsMoving = true; }
		if (KEY_PRESSING(DIK_A)) { vMovementDirection -= projectedCamRight; bIsMoving = true; }
		if (KEY_PRESSING(DIK_D)) { vMovementDirection += projectedCamRight; bIsMoving = true; }

		// 벡터의 길이가 0이 아닌 경우에만 정규화
		if (XMVectorGetX(XMVector3LengthSq(vMovementDirection)) > 0.001f) {
			vMovementDirection = XMVector3Normalize(vMovementDirection);
		}

		if (KEY_PRESSING(DIK_LSHIFT))
		{
			m_eMovementState = EMovementState::BOOST;
			if (!bIsMoving)
				XMStoreFloat3(&status.vMoveDirection, XMVector4Normalize(pTransform->Get_State(STATE::LOOK)));
			else
				XMStoreFloat3(&status.vMoveDirection, vMovementDirection);
			return;
		}

		if (KEY_PRESSING(DIK_Q))
		{
			XMStoreFloat3(&status.vMoveDirection, -XMVector3Normalize(pTransform->Get_State(STATE::RIGHT)));
			m_eMovementState = m_eMovementState == EMovementState::IDLE ? EMovementState::AVOID : EMovementState::QUICKSTEP;
			return;
		}

		if (KEY_PRESSING(DIK_E))
		{
			XMStoreFloat3(&status.vMoveDirection, XMVector3Normalize(pTransform->Get_State(STATE::RIGHT)));
			m_eMovementState = m_eMovementState == EMovementState::IDLE ? EMovementState::AVOID : EMovementState::QUICKSTEP;
			return;
		}

		if (KEY_DOWN(DIK_SPACE))
		{
			if (status.bCanJump)
			{
				m_eMovementState = EMovementState::JUMP;
				// 초기 수직 점프 속도 설정
				status.fVerticalVelocity.first = status.fJumpForce;
				status.bOnGround = false; // 공중 상태로 전환
				status.bCanJump = false;
				status.bCanDoubleJump = true;
				return;
			}
		}

		if (KEY_DOWN(DIK_I))
		{
			pController->Change_State(ENUM_CLASS(CSonic_Controller::States::COMBAT), ENUM_CLASS(CSonic::STATE::COMBO_KICK01) + status.iCombos.first++, 1.5f, false);
			return;
		}

		if (bIsMoving)
		{
			XMStoreFloat3(&status.vMoveDirection, vMovementDirection);
			if (status.fMoveSpeed.first >= 1.5f && status.fMoveSpeed.first < 8.5f)
			{
				m_eMovementState = EMovementState::RUN;
			}
			else if (status.fMoveSpeed.first >= 8.5f)
			{
				m_eMovementState = EMovementState::DASH;
			}
			else
			{
				m_eMovementState = EMovementState::WALK;
			}
		}
		else
		{
			if (status.fMoveSpeed.first >= 2.f)
			{
				m_eMovementState = EMovementState::BRAKE;
			}
			else
			{
				if (pAnimator->Get_AnimationProgress() >= 0.75f || m_eMovementState == EMovementState::WALK)
				{
					m_eMovementState = EMovementState::IDLE;
				}
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

#pragma region Interaction
void CSonic_Movement_State::Begin_Spline()
{
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	if (!pController) return;
	CSpline_Component* pSpline = pController->Get_CurrentSpline();
	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();

	CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	if (status.bIsOnSpline)
		return;

	_vector vSonicPosition = pTransform->Get_State(STATE::POSITION);
	_float fClosestT = pSpline->Find_Closest_Param(vSonicPosition);

	// 상태 설정
	status.fSplineParameter = fClosestT;
	status.bIsOnSpline = true;
	m_eMovementState = EMovementState::RIDER;
	static_cast<CSonic*>(m_pOwner)->Change_ModelIndex(0);
	pController->Change_State(ENUM_CLASS(CSonic_Controller::States::MOVE), ENUM_CLASS(CSonic::STATE::GRIND_LOOP));

	// 레일 진행 방향 설정 (방향은 플레이어의 현재 속도 방향과 레일 방향 중 더 일치하는 쪽으로)
	Determine_SplineDirection();

	// 캐릭터를 레일 위에 정확히 배치
	Update_PositionOnSpline();
}
void CSonic_Movement_State::Update_SplineMovement(_float fTimeDelta)
{
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	if (!pController) return;
	CSpline_Component* pSpline = pController->Get_CurrentSpline();
	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();

	if (!pSpline || !status.bIsOnSpline)
		return;

	// 레일 위에서 이동 속도 (현재 속도 유지 또는 고정값 사용)
	_float fRailSpeed = status.fMoveSpeed.first;
	if (fRailSpeed < 5.f) fRailSpeed = 5.f; // 최소 속도 보장

	// 스플라인의 전체 길이를 기준으로 이동량 계산
	_float fSplineLength = pSpline->GetTotalLength();
	_float fDeltaT = fRailSpeed / fSplineLength * fTimeDelta;

	// 진행 방향에 따라 파라미터 업데이트
	if (status.bIsForward)
	{
		status.fSplineParameter += fDeltaT;
	}
	else
	{
		status.fSplineParameter -= fDeltaT;
	}

	// 레일 끝에 도달했는지 확인
	if (status.fSplineParameter >= 1.f || status.fSplineParameter <= 0.f)
	{
		status.bIsOnSpline = false;
		pController->Unregister_Spline();

		// 초기 수직 점프 속도 설정
		status.fVerticalVelocity.first = status.fJumpForce;
		status.bCanJump = false;   // 레일에서 떨어지면 점프 가능
		status.bOnGround = false; // 공중 상태로 전환
		status.bCanDoubleJump = true;
		m_eMovementState = EMovementState::JUMP;
		return;
	}

	// 새 위치로 업데이트
	Update_PositionOnSpline();
}
void CSonic_Movement_State::Update_PositionOnSpline()
{
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	if (!pController) return;
	CSpline_Component* pSpline = pController->Get_CurrentSpline();
	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();

	if (!pSpline || !status.bIsOnSpline)
		return;

	vector<CSpline_Component::SPLINE_FRAME>& splineFrames = pSpline->Get_SplineFrames();
	CTransform* pSplineTransform = static_cast<CTransform*>(pSpline->Get_OwnerGameObject()->Get_Component(g_strTransformTag));
	if (splineFrames.empty())
		return;

	// 스플라인 파라미터를 기반으로 정확한 위치 계산
	_float fTotalFrames = static_cast<_float>(splineFrames.size() - 1);
	_float fExactFramePos = status.fSplineParameter * fTotalFrames;

	// 보간을 위한 두 프레임 인덱스 계산
	_uint uIndex1 = static_cast<_uint>(fExactFramePos);
	_uint uIndex2 = uIndex1 + 1;

	// 인덱스가 범위를 벗어나지 않도록 제한
	uIndex1 = min(uIndex1, static_cast<_uint>(splineFrames.size() - 1));
	uIndex2 = min(uIndex2, static_cast<_uint>(splineFrames.size() - 1));

	// 두 프레임 간의 보간 계수 계산
	_float fLerpFactor = fExactFramePos - uIndex1;

	// 레일 위 위치 설정
	CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	if (!pTransform)
		return;

	// 스플라인 오브젝트의 월드 행렬을 가져옵니다.
	_matrix railTransform = pSplineTransform->Get_WorldMatrix();

	// 로컬 공간에서 위치, 탄젠트, 노멀 벡터를 선형 보간합니다.
	_vector vLocalPosition = XMVectorLerp(XMLoadFloat3(&splineFrames[uIndex1].vPosition), XMLoadFloat3(&splineFrames[uIndex2].vPosition), fLerpFactor);
	_vector vLocalTangent = XMVectorLerp(XMLoadFloat3(&splineFrames[uIndex1].vTangent), XMLoadFloat3(&splineFrames[uIndex2].vTangent), fLerpFactor);
	_vector vLocalNormal = XMVectorLerp(XMLoadFloat3(&splineFrames[uIndex1].vNormal), XMLoadFloat3(&splineFrames[uIndex2].vNormal), fLerpFactor);

	// 보간된 로컬 벡터들을 월드 공간으로 변환합니다.
	_vector vWorldPosition = XMVector3TransformCoord(vLocalPosition, railTransform);
	_vector vWorldTangent = XMVector3Normalize(XMVector3TransformNormal(vLocalTangent, railTransform));
	_vector vWorldNormal = XMVector3Normalize(XMVector3TransformNormal(vLocalNormal, railTransform));

	// 캐릭터의 위치를 월드 좌표로 설정합니다.
	pTransform->Set_State(STATE::POSITION, vWorldPosition);

	// 1. 최종 진행 방향(Look)을 월드 탄젠트로 결정합니다.
	_vector vFinalLook = vWorldTangent;
	if (!status.bIsForward)
	{
		vFinalLook = XMVectorNegate(vFinalLook);
	}

	// 2. 스플라인의 노멀을 사용하되, 불안정한 경우를 대비하여 안정적인 Up 벡터를 계산합니다.
	_vector vFinalUp;
	_vector vFinalRight;

	// 스플라인의 노멀 벡터와 Look 벡터가 거의 평행한지 확인합니다.
	// 내적의 절댓값이 0.99 이상이면 거의 평행하다고 간주합니다.
	if (abs(XMVectorGetX(XMVector3Dot(vFinalLook, vWorldNormal))) > 0.99f)
	{
		// 평행한 경우: 월드 Up 벡터를 기준으로 새로운 직교 기저를 생성합니다.
		// 이는 주로 수직 루프의 최상단/최하단에서 발생할 수 있는 문제를 방지합니다.
		_vector vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		vFinalRight = XMVector3Normalize(XMVector3Cross(vWorldUp, vFinalLook));
		vFinalUp = XMVector3Normalize(XMVector3Cross(vFinalLook, vFinalRight));
	}
	else
	{
		// 일반적인 경우: 스플라인의 노멀을 기준으로 직교 기저를 생성합니다.
		vFinalRight = XMVector3Normalize(XMVector3Cross(vWorldNormal, vFinalLook));
		vFinalUp = XMVector3Normalize(XMVector3Cross(vFinalLook, vFinalRight));
	}

	// 상태 정보 업데이트
	XMStoreFloat3(&status.vMoveDirection, vFinalLook);

	// 소닉의 최종 방향 설정
	pTransform->Set_State(STATE::LOOK, vFinalLook);
	pTransform->Set_State(STATE::UP, vFinalUp);
	pTransform->Set_State(STATE::RIGHT, vFinalRight);
}
void CSonic_Movement_State::Determine_SplineDirection()
{
	CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	if (!pController) return;
	CSpline_Component* pSpline = pController->Get_CurrentSpline();
	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();

	if (!pSpline || !status.bIsOnSpline)
		return;

	// 현재 위치에서의 스플라인 탄젠트 벡터 가져오기
	vector<CSpline_Component::SPLINE_FRAME>& splineFrames = pSpline->Get_SplineFrames();
	if (splineFrames.empty())
		return;

	// 현재 파라미터에 해당하는 프레임 인덱스 계산
	_uint frameIdx = (_uint)(status.fSplineParameter * (splineFrames.size() - 1));
	if (frameIdx >= splineFrames.size())
		frameIdx = splineFrames.size() - 1;

	// 플레이어의 현재 이동 방향과 레일 방향 비교
	_vector playerDir = XMLoadFloat3(&status.vMoveDirection);
	_vector railTangent = XMLoadFloat3(&splineFrames[frameIdx].vTangent);

	// 두 방향의 내적으로 방향 일치도 확인
	_float dotProduct = XMVectorGetX(XMVector3Dot(playerDir, railTangent));

	// 내적이 음수면 레일 방향과 반대 방향으로 진행 중
	status.bIsForward = (dotProduct >= 0.0f);
}
#pragma endregion
