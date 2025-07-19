#pragma once
#include "Sonic_Controller.h"
#include "Sonic.h"
#include "State.h"
#include "Sonic_States.h"
#include "Spline_Component.h"
#include "Animator.h"
#include "Sonic.h"
#include "Sonic_Movement_State.h"
#include "GameInstance.h"

CSonic_Controller::CSonic_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CController{ pDevice, pContext }
{
}

CSonic_Controller::CSonic_Controller(const CSonic_Controller& Prototype)
	: CController{ Prototype }
{
	m_iCurStateIndex = 0;
}

HRESULT CSonic_Controller::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CSonic_Controller::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//m_pPxController = 

	return S_OK;
}
HRESULT CSonic_Controller::Late_Initialize()
{
	return S_OK;
}

CSonic_Controller* CSonic_Controller::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSonic_Controller* pInstance = new CSonic_Controller(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSonic_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CComponent* CSonic_Controller::Clone(void* pArg)
{
	CSonic_Controller* pInstance = new CSonic_Controller(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CSonic_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CSonic_Controller::Free()
{
	__super::Free();
}

HRESULT CSonic_Controller::Serialize(json& data)
{
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	return S_OK;
}
HRESULT CSonic_Controller::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

#pragma region Loops
void CSonic_Controller::Priority_Update(_float fTimeDelta)
{
}
void CSonic_Controller::Update(_float fTimeDelta)
{
	UpdateInputBuffer(fTimeDelta);
	if (m_States[m_iCurStateIndex])
	{
		m_States[m_iCurStateIndex]->Execute(fTimeDelta);
		m_States[m_iCurStateIndex]->HandleInput(fTimeDelta);
	}

	Update_Targeting(fTimeDelta);
	if (m_pTarget)
	{
		if (MOUSE_DOWN(DIM::LBUTTON))
		{
			HomingAttack();
		}
	}
}
void CSonic_Controller::Late_Update(_float fTimeDelta)
{
}
HRESULT CSonic_Controller::Render()
{
	return S_OK;
}
// Sonic_Controller.cpp 에 새 함수 추가
void CSonic_Controller::Update_Targeting(_float fTimeDelta)
{
	// 락온 키 (예: 마우스 휠 버튼)를 누르면 타겟 탐색/해제
	if (!m_tInfo.bOnGround) // 'R' 키를 락온 키로 가정
	{
		m_pTarget = FindBestTarget();
	}
	else
	{
		m_pTarget = nullptr;
	}

	// 메인 카메라(Follow_Camera)에 타겟 정보 전달
	CCamera* pMainCamera = m_pGameInstance->Get_Current_Camera();
	CFollow_Camera* pFollowCamera = dynamic_cast<CFollow_Camera*>(pMainCamera);
	if (pFollowCamera)
	{
		pFollowCamera->Set_LockOn_Target(m_pTarget); // 락온 타겟 설정
	}
}

physx::PxTransform ToPxTransform(const _float4x4& matrix) {
	physx::PxMat44 pxMat(reinterpret_cast<const physx::PxMat44&>(matrix));
	return physx::PxTransform(pxMat);
}

CGameObject* CSonic_Controller::FindBestTarget()
{
	CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));

	// 1. 플레이어 주변의 후보군 탐색
	PxSphereGeometry sphereGeom(m_tInfo.fDetectionRange);
	PxTransform transform = ToPxTransform(pTransform->Get_World4x4());

	// 필터 설정 (예: ENEMY 레이어만 탐지)
	PxQueryFilterData filterData;
	filterData.data.word3 = static_cast<PxU32>(2);

	vector<PxOverlapHit> hits = m_pGameInstance->Get_OverlapHits(sphereGeom, transform, filterData);

	if (hits.empty())
		return nullptr;

	// 2. 최고 점수 타겟 선정
	CGameObject* pBestTarget = nullptr;
	float fBestScore = -1.f;

	_vector vPlayerPos = pTransform->Get_State(STATE::POSITION);
	_matrix matView = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);

	for (const auto& hit : hits)
	{
		CGameObject* pCandidate = reinterpret_cast<CGameObject*>(hit.actor->userData);
		if (!pCandidate || pCandidate == m_pOwner)
			continue;

		CTransform* pCandidateTransform = static_cast<CTransform*>(pCandidate->Get_Component(g_strTransformTag));
		_vector vCandidatePos = pCandidateTransform->Get_State(STATE::POSITION);

		// 화면 좌표로 변환하여 화면 중앙과의 거리 계산
		_vector vPosInView = XMVector3TransformCoord(vCandidatePos, matView);

		// 뷰 프러스텀 바깥에 있으면 제외
		if (vPosInView.m128_f32[2] < 0) continue;

		_float2 vPosInView2D = _float2(vPosInView.m128_f32[0], vPosInView.m128_f32[1]);

		_float fDistFromCenter = XMVectorGetX(XMVector2Length(XMLoadFloat2(&vPosInView2D)));
		_float fDistFromPlayer = XMVectorGetX(XMVector3Length(vCandidatePos - vPlayerPos));

		// 점수 계산 (화면 중앙에 가까울수록, 플레이어에 가까울수록 높은 점수)
		float fScore = (1.f / (fDistFromCenter + 0.1f)) + (1.f / (fDistFromPlayer + 0.1f));

		if (fScore > fBestScore)
		{
			fBestScore = fScore;
			pBestTarget = pCandidate;
		}
	}

	return pBestTarget;
}
void CSonic_Controller::Change_State(_uint iIndex)
{
	if (iIndex >= m_States.size())
		return;

	if (m_pOwner)
	{
		m_States[m_iCurStateIndex]->Exit();
		m_iCurStateIndex = iIndex;
		m_States[m_iCurStateIndex]->Enter();
		CAnimator* pAnimator = static_cast<CAnimator*>(m_pOwner->Get_Component(L"Com_Animator"));
		if (pAnimator)
		{
			_bool bLoop = m_States[m_iCurStateIndex]->Get_IsLoop();
			_float fPlaybackSpeed = m_tInfo.fMoveSpeed.first <= 1.5f ? 1.5f : m_tInfo.fMoveSpeed.first;
			_float fBlendDuration = m_States[m_iCurStateIndex]->Get_BlendDuration();

			pAnimator->Change_Animation(m_iCurStateIndex, bLoop, fPlaybackSpeed, fBlendDuration);
		}
	}
}
void CSonic_Controller::Change_State(_uint iStateIndex, _uint iAnimIndex, _float fPlaybackSpeed, _bool bIsLoop, _float fBlendDuration)
{
	if (iStateIndex >= m_States.size())
		return;

	if (m_pOwner)
	{
		m_States[m_iCurStateIndex]->Exit();
		m_iCurStateIndex = iStateIndex;
		m_States[m_iCurStateIndex]->Enter();
 		CAnimator* pAnimator = static_cast<CAnimator*>(m_pOwner->Get_Component(L"Com_Animator"));
		if (pAnimator)
		{
			pAnimator->Change_Animation(iAnimIndex, fPlaybackSpeed, bIsLoop, fBlendDuration);
		}
	}
}
#pragma endregion
HRESULT CSonic_Controller::RenderImGui()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Sonic Status Controller", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		if (m_States[m_iCurStateIndex])
			m_States[m_iCurStateIndex]->RenderImGui();

		ImGui::Separator();

		if (ImGui::TreeNode("Default Movement"))
		{
			ImGui::InputFloat3("Move Direction", &m_tInfo.vMoveDirection.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::Text("Move Speed: %.2f / %.2f", m_tInfo.fMoveSpeed.first, m_tInfo.fMoveSpeed.second);
			ImGui::InputFloat("Move Acceleration", &m_tInfo.fMoveAccelration, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat("Rotation Speed", &m_tInfo.fRotationSpeed, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);

			ImGui::Checkbox("Can Jump", &m_tInfo.bCanJump);
			ImGui::Checkbox("Can Double Jump", &m_tInfo.bCanDoubleJump);
			ImGui::Checkbox("Sliding", &m_tInfo.bSliding);

			ImGui::SliderFloat("Friction", &m_tInfo.fFriction, 0.f, 1.f, "%.2f");

			ImGui::Text("Boost Gauge: %.2f / %.2f", m_tInfo.fBoostGauge.first, m_tInfo.fBoostGauge.second);
			ImGui::ProgressBar(m_tInfo.fBoostGauge.first / m_tInfo.fBoostGauge.second, ImVec2(0.0f, 0.0f));
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
			ImGui::Text("Boost");

			ImGui::Text("Current Slope Angle %.2f", m_tInfo.fCurSlope);
			ImGui::InputFloat("Slope Limit Angle", &m_tInfo.fSlopeLimitAngle, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::TreePop();
		}
		ImGui::Separator();

		if (ImGui::TreeNode("Combat"))
		{
			ImGui::Text("Attack Damage: %.2f - %.2f", m_tInfo.fAtkDamage.first, m_tInfo.fAtkDamage.second);
			ImGui::InputFloat("Combo Delay", &m_tInfo.fComboDelay, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat("Hit Delay", &m_tInfo.fHitDelay, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::Checkbox("Can Hit", &m_tInfo.bCanHit);
			ImGui::Text("Combo Count: %u / % u", m_tInfo.iCombos.first, m_tInfo.iCombos.second); // _uint는 %u로 표시

			ImGui::SeparatorText("Input Buffering");
			ImGui::Checkbox("Combo Input Buffered", &m_tInfo.bComboInputBuffered);
			ImGui::InputFloat("Buffer Timer", &m_tInfo.fComboInputBufferTimer, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat("Buffer Duration (Const)", (float*)&m_tInfo.fComboInputBufferDuration, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly); // const_cast 대신 포인터 캐스팅
			ImGui::TreePop();
		}
		ImGui::Separator();

		if (ImGui::TreeNode("Status"))
		{
			ImGui::Text("Rings: %u / %u", m_tInfo.iRings.first, m_tInfo.iRings.second); // _uint는 %u로 표시
			ImGui::ProgressBar((float)m_tInfo.iRings.first / m_tInfo.iRings.second, ImVec2(0.0f, 0.0f));
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
			ImGui::Text("Rings");
			ImGui::TreePop();
		}

		ImGui::Unindent();
	}
	ImGui::PopID();
	return S_OK;
}

#pragma region Combo / Combat
void CSonic_Controller::BufferComboInput()
{
	m_tInfo.bComboInputBuffered = true;
	m_tInfo.fComboInputBufferTimer = m_tInfo.fComboInputBufferDuration; // 타이머 초기화
}

_bool CSonic_Controller::ConsumeComboInput()
{
	if (m_tInfo.bComboInputBuffered)
	{
		m_tInfo.bComboInputBuffered = false;
		m_tInfo.fComboInputBufferTimer = 0.f;
		return true;
	}
	return false;
}

void CSonic_Controller::ClearComboInputBuffer()
{
	m_tInfo.bComboInputBuffered = false;
	m_tInfo.fComboInputBufferTimer = 0.f;
}

void CSonic_Controller::UpdateInputBuffer(_float fTimeDelta)
{
	if (m_tInfo.bComboInputBuffered)
	{
		m_tInfo.fComboInputBufferTimer -= fTimeDelta;
		if (m_tInfo.fComboInputBufferTimer <= 0.f)
		{
			ClearComboInputBuffer(); // 유효 시간 초과 시 버퍼 초기화
		}
	}
}
void CSonic_Controller::HomingAttack()
{
	CSonic_Movement_State* pMovementState = static_cast<CSonic_Movement_State*>(m_States[ENUM_CLASS(States::MOVE)]);
	if (pMovementState)
	{
		m_pGameInstance->Play_Event(L"event:/sn_homing")->SetVolume(0.3f);
		// CSonic_Movement_State의 EMovementState 열거형에 HOMING_ATTACK을 추가해야 합니다.
		pMovementState->Set_MovementState(CSonic_Movement_State::EMovementState::HOMING_ATTACK);
	}
}
void CSonic_Controller::ReboundFromAttack()
{
	// 타겟을 파괴한 후 위로 튀어오르는 효과
	m_tInfo.fVerticalVelocity.first = m_tInfo.fJumpForce * 0.8f; // 점프 힘의 80% 정도로 튀어오름
	m_tInfo.bCanJump = false;
	m_tInfo.bCanDoubleJump = true; // 리바운드 후 더블 점프 가능

	// 락온 타겟 해제
	if (m_pTarget)
	{
		Safe_Release(m_pTarget);
		m_pTarget = nullptr;
	}

	// 상태를 일반 점프(공중) 상태로 되돌림
	CSonic_Movement_State* pMovementState = static_cast<CSonic_Movement_State*>(m_States[ENUM_CLASS(States::MOVE)]);
	if (pMovementState)
	{
		pMovementState->Set_MovementState(CSonic_Movement_State::EMovementState::JUMP);
	}
}
#pragma endregion

void CSonic_Controller::Spring(_fvector vPushDirection)
{
	m_tInfo.bIsSpring = true;
	m_tInfo.bOnGround = false;
	m_tInfo.bCanJump = false;
	m_tInfo.bCanDoubleJump = true;
	const float fSpringPower = 15.f; // 튕겨나가는 힘의 크기 (조절 가능)
	_vector vInitialVelocity = XMVector3Normalize(vPushDirection) * fSpringPower;

	// 속도를 수직/수평 성분으로 분해
	_vector vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vVerticalComponent = XMVector3Dot(vInitialVelocity, vWorldUp) * vWorldUp;
	_vector vHorizontalComponent = vInitialVelocity - vVerticalComponent;

	// 수직 속도 설정
	m_tInfo.fVerticalVelocity.first = XMVectorGetX(XMVector3Length(vVerticalComponent));
	// 만약 밀어내는 방향의 y값이 음수이면, 수직 속도도 음수로 설정
	if (XMVectorGetY(vPushDirection) < 0.f)
	{
		m_tInfo.fVerticalVelocity.first *= -1.f;
	}

	// 수평 속도 및 방향 설정
	m_tInfo.fMoveSpeed.first = XMVectorGetX(XMVector3Length(vHorizontalComponent));
	XMStoreFloat3(&m_tInfo.vMoveDirection, XMVector3Normalize(vHorizontalComponent));
}

void CSonic_Controller::DashPanel(_fvector vDashDirection)
{
	m_tInfo.fMoveSpeed.first = m_tInfo.fMaxBoostSpeed * 3.f;
	XMStoreFloat3(&m_tInfo.vMoveDirection, XMVector3Normalize(vDashDirection));
}

void CSonic_Controller::DashRing(_fvector vDashDirection)
{
	static_cast<CSonic_Movement_State*>(m_States[m_iCurStateIndex])->Set_MovementState(CSonic_Movement_State::EMovementState::DASHRING);
	m_tInfo.bIsSpring = false;
	m_tInfo.bOnGround = false;
	m_tInfo.bCanJump = false;
	m_tInfo.bCanDoubleJump = false;
	const float fSpringPower = 50.f; // 튕겨나가는 힘의 크기 (조절 가능)
	_vector vInitialVelocity = XMVector3Normalize(vDashDirection) * fSpringPower;

	// 속도를 수직/수평 성분으로 분해
	_vector vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vVerticalComponent = XMVector3Dot(vInitialVelocity, vWorldUp) * vWorldUp;
	_vector vHorizontalComponent = vInitialVelocity - vVerticalComponent;

	// 수직 속도 설정
	m_tInfo.fVerticalVelocity.first = XMVectorGetX(XMVector3Length(vVerticalComponent)) * 3.f;
	// 만약 밀어내는 방향의 y값이 음수이면, 수직 속도도 음수로 설정
	if (XMVectorGetY(vDashDirection) < 0.f)
	{
		m_tInfo.fVerticalVelocity.first *= -1.f;
	}

	// 수평 속도 및 방향 설정
	m_tInfo.fMoveSpeed.first = XMVectorGetX(XMVector3Length(vHorizontalComponent));
	XMStoreFloat3(&m_tInfo.vMoveDirection, XMVector3Normalize(vHorizontalComponent));
}

void CSonic_Controller::Begin_Pulley()
{

}


void CSonic_Controller::Begin_Dashboard()
{

}

#pragma region Ring
void CSonic_Controller::Add_Ring(_uint iNumber)
{
	m_tInfo.iRings.first += iNumber;
	if (m_tInfo.iRings.first > m_tInfo.iRings.second)
	{
		m_tInfo.iRings.first = m_tInfo.iRings.second;
	}
}

void CSonic_Controller::Subtract_Ring(_uint iNumber)
{
	m_tInfo.iRings.first -= iNumber;
	if (m_tInfo.iRings.first < 0)
	{
		m_tInfo.iRings.first = 0;
	}
}
#pragma endregion