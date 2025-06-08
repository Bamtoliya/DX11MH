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
	
	/*if (pRigidBody)
	{
		_float3 vCurrentVelocity = pRigidBody->Get_Linear_Velocity();
		m_tStateParams.fMoveSpeed = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vCurrentVelocity)));
	}*/
}

void CSonic_Ground_State::HandleInput()
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

	if (KEY_DOWN(DIK_SPACE))
	{
		m_tStateParams.bCanJump = false;
		//pController->Change_State(ENUM_CLASS(CSonic::STATE::JUMP_U));
	}
		
		

	if (bIsMoving)
	{
		vMovementDirection = XMVector3Normalize(vMovementDirection); // 최종 이동 방향 정규화

		// 캐릭터가 이동할 방향으로 회전 (이 부분은 유지하거나 물리 기반 회전으로 변경 가능)
		_float fTargetAngleRadians = atan2f(XMVectorGetX(vMovementDirection), XMVectorGetZ(vMovementDirection));
		pTransform->Turn_Quaternion(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTargetAngleRadians);

		// 캐릭터 이동 (물리 기반으로 변경)
		_float3 vForceDirection;
		XMStoreFloat3(&vForceDirection, vMovementDirection);

		// 적용할 힘의 크기 (m_tStateParams.fMoveSpeed는 상태별 목표 속도이므로, 가속도로 변환하거나 직접 힘의 크기를 설정)
		// 예시: 일정한 가속도 적용
		_float fAccelerationMagnitude = 20.f; // 예시 가속도 값, 필요에 따라 조절
		//if (pController->Get_CurStateIndex() == ENUM_CLASS(CSonic::STATE::WALK_LOOP))
		//	fAccelerationMagnitude = 10.f; // 걷기 상태일 때 가속도
		//else if (pController->Get_CurStateIndex() == ENUM_CLASS(CSonic::STATE::RUN_LOOP))
		//	fAccelerationMagnitude = 20.f; // 달리기 상태일 때 가속도
		//else if (pController->Get_CurStateIndex() == ENUM_CLASS(CSonic::STATE::BOOST_LOOP))
		//	fAccelerationMagnitude = 30.f; // 부스트 상태일 때 가속도


		_float3 vAppliedForce = { vForceDirection.x * fAccelerationMagnitude, 0.f, vForceDirection.z * fAccelerationMagnitude };
		// Y축 힘은 중력이나 점프 로직에서 별도 처리 가정

		pRigidBody->Add_Force(vAppliedForce, physx::PxForceMode::eVELOCITY_CHANGE);


		// 상태 전환 로직 (현재 속도에 따라 상태 변경)
		// Execute 함수에서 현재 속도를 기반으로 m_tStateParams.fMoveSpeed를 업데이트하고,
		// 그 값을 기준으로 상태를 변경하도록 할 수 있습니다.
		// 또는 여기서 직접 pRigidBody->Get_Linear_Velocity()를 사용하여 상태 변경
		_float3 currentVelocity = pRigidBody->Get_Linear_Velocity();
		_float currentSpeed = XMVectorGetX(XMVector3Length(XMLoadFloat3(&currentVelocity)));

		//if (pController->Get_CurStateIndex() != ENUM_CLASS(CSonic::STATE::BOOST_LOOP)) // 부스트 중이 아닐 때만 속도 기반 상태 변경
		//{
			if (currentSpeed >= 10.f) // 이 값은 m_tStateParams.fMoveSpeed와 연동되거나, 별도 임계값 사용
				pController->Change_State(ENUM_CLASS(CSonic::STATE::RUN_LOOP));
			else
				pController->Change_State(ENUM_CLASS(CSonic::STATE::WALK_LOOP));
		//}
	}
	else
	{
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
	m_tStateParams.fMoveSpeed = 0.f;
	/*CRigidBody* pRigidBody = static_cast<CRigidBody*>(m_pOwner->Get_Component(L"Com_RigidBody"));
	pRigidBody->Set_Linear_Velocity(_float3(0.f, 0.f, 0.f));*/
}

void CSonic_Idle_State::HandleInput()
{
	__super::HandleInput();
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
	if (m_tStateParams.fMoveSpeed < 10.f)
		m_tStateParams.fMoveSpeed += fTimeDelta;
}

void CSonic_Walk_State::HandleInput()
{
	__super::HandleInput();
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
	if(m_tStateParams.fMoveSpeed > 10.f)
		m_tStateParams.fMoveSpeed -= fTimeDelta;

	if (m_tStateParams.fMoveSpeed < 10.f)
		m_tStateParams.fMoveSpeed = 10.f;

}

void CSonic_Run_State::HandleInput()
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

		// 캐릭터가 이동할 방향으로 회전 (이 부분은 유지하거나 물리 기반 회전으로 변경 가능)
		_float fTargetAngleRadians = atan2f(XMVectorGetX(vMovementDirection), XMVectorGetZ(vMovementDirection));
		pTransform->Turn_Quaternion(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTargetAngleRadians);

		// 캐릭터 이동 (물리 기반으로 변경)
		_float3 vForceDirection;
		XMStoreFloat3(&vForceDirection, vMovementDirection);

		// 적용할 힘의 크기 (m_tStateParams.fMoveSpeed는 상태별 목표 속도이므로, 가속도로 변환하거나 직접 힘의 크기를 설정)
		// 예시: 일정한 가속도 적용
		_float fAccelerationMagnitude = 20.f; // 예시 가속도 값, 필요에 따라 조절
		//if (pController->Get_CurStateIndex() == ENUM_CLASS(CSonic::STATE::WALK_LOOP))
		//	fAccelerationMagnitude = 10.f; // 걷기 상태일 때 가속도
		//else if (pController->Get_CurStateIndex() == ENUM_CLASS(CSonic::STATE::RUN_LOOP))
		//	fAccelerationMagnitude = 20.f; // 달리기 상태일 때 가속도
		//else if (pController->Get_CurStateIndex() == ENUM_CLASS(CSonic::STATE::BOOST_LOOP))
		//	fAccelerationMagnitude = 30.f; // 부스트 상태일 때 가속도


		_float3 vAppliedForce = { vForceDirection.x * fAccelerationMagnitude, 0.f, vForceDirection.z * fAccelerationMagnitude };
		// Y축 힘은 중력이나 점프 로직에서 별도 처리 가정

		pRigidBody->Add_Force(vAppliedForce, physx::PxForceMode::eACCELERATION);


		// 상태 전환 로직 (현재 속도에 따라 상태 변경)
		// Execute 함수에서 현재 속도를 기반으로 m_tStateParams.fMoveSpeed를 업데이트하고,
		// 그 값을 기준으로 상태를 변경하도록 할 수 있습니다.
		// 또는 여기서 직접 pRigidBody->Get_Linear_Velocity()를 사용하여 상태 변경
		_float3 currentVelocity = pRigidBody->Get_Linear_Velocity();
		_float currentSpeed = XMVectorGetX(XMVector3Length(XMLoadFloat3(&currentVelocity)));

		//if (pController->Get_CurStateIndex() != ENUM_CLASS(CSonic::STATE::BOOST_LOOP)) // 부스트 중이 아닐 때만 속도 기반 상태 변경
		//{
		if (currentSpeed >= 10.f) // 이 값은 m_tStateParams.fMoveSpeed와 연동되거나, 별도 임계값 사용
			pController->Change_State(ENUM_CLASS(CSonic::STATE::RUN_LOOP));
		else
			pController->Change_State(ENUM_CLASS(CSonic::STATE::WALK_LOOP));
		//}
	}
	else
	{
		pController->Change_State(ENUM_CLASS(CSonic::STATE::IDLE_LOOP));
	}
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

void CSonic_Boost_State::HandleInput()
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

void CSonic_Brake_State::HandleInput()
{
	//__super::HandleInput();
}

void CSonic_Brake_State::Exit()
{
}

#pragma endregion


