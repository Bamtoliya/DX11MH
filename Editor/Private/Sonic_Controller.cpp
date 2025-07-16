#include "Sonic_Controller.h"
#include "Sonic.h"
#include "State.h"
#include "Sonic_States.h"
#include "Spline_Component.h"
#include "Animator.h"
#include "Sonic.h"

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
}
void CSonic_Controller::Late_Update(_float fTimeDelta)
{
}
HRESULT CSonic_Controller::Render()
{
	return S_OK;
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
#pragma endregion

void CSonic_Controller::Begin_Rail(CSpline_Component* pSpline, _float fStartT)
{
	if (m_tInfo.bIsOnSpline)
		return;

	// 스플라인 컴포넌트 저장 및 파라미터 초기화
 	m_pCurrentSpline = pSpline;
	m_tInfo.fSplineParameter = fStartT;
	m_tInfo.bIsOnSpline = true;

	// 레일 진행 방향 설정 (방향은 플레이어의 현재 속도 방향과 레일 방향 중 더 일치하는 쪽으로)
	DetermineSplineDirection();

	// 레일 상태로 전환 (예: 그라인드 애니메이션)
	static_cast<CSonic*>(m_pOwner)->Change_ModelIndex(0);
	Change_State(ENUM_CLASS(States::MOVE), ENUM_CLASS(CSonic::STATE::GRIND_LOOP), m_tInfo.fMoveSpeed.first);

	// 캐릭터를 레일 위에 정확히 배치
	UpdatePositionOnSpline();
}

void CSonic_Controller::Begin_Pulley()
{

}

void CSonic_Controller::Begin_Spring()
{

}

void CSonic_Controller::Begin_Dashboard()
{

}

void CSonic_Controller::DetermineSplineDirection()
{
	if (!m_pCurrentSpline || !m_tInfo.bIsOnSpline)
		return;

	// 현재 위치에서의 스플라인 탄젠트 벡터 가져오기
	vector<CSpline_Component::SPLINE_FRAME>& splineFrames = m_pCurrentSpline->Get_SplineFrames();
	if (splineFrames.empty())
		return;

	// 현재 파라미터에 해당하는 프레임 인덱스 계산
	_uint frameIdx = (_uint)(m_tInfo.fSplineParameter * (splineFrames.size() - 1));
	if (frameIdx >= splineFrames.size())
		frameIdx = splineFrames.size() - 1;

	// 플레이어의 현재 이동 방향과 레일 방향 비교
	_vector playerDir = XMLoadFloat3(&m_tInfo.vMoveDirection);
	_vector railTangent = XMLoadFloat3(&splineFrames[frameIdx].vTangent);

	// 두 방향의 내적으로 방향 일치도 확인
	_float dotProduct = XMVectorGetX(XMVector3Dot(playerDir, railTangent));

	// 내적이 음수면 레일 방향과 반대 방향으로 진행 중
	m_tInfo.bIsForward = (dotProduct >= 0.0f);
}

void CSonic_Controller::Update_RailMovement(_float fTimeDelta)
{
	static_cast<CSonic*>(m_pOwner)->Change_ModelIndex(0);
	Change_State(ENUM_CLASS(States::MOVE), ENUM_CLASS(CSonic::STATE::GRIND_LOOP), m_tInfo.fMoveSpeed.first);
	if (!m_pCurrentSpline || !m_tInfo.bIsOnSpline)
		return;

	// 레일 위에서 이동 속도 (현재 속도 유지 또는 고정값 사용)
	_float fRailSpeed = m_tInfo.fMoveSpeed.first;
	if (fRailSpeed < 5.f) fRailSpeed = 5.f; // 최소 속도 보장

	// 스플라인의 전체 길이를 기준으로 이동량 계산
	_float fSplineLength = m_pCurrentSpline->GetTotalLength();
	_float fDeltaT = fRailSpeed / fSplineLength * fTimeDelta;

	// 진행 방향에 따라 파라미터 업데이트
	if (m_tInfo.bIsForward)
	{
		m_tInfo.fSplineParameter += fDeltaT;
	}
	else
	{
		m_tInfo.fSplineParameter -= fDeltaT;
	}

	// 레일 끝에 도달했는지 확인
	if (m_tInfo.fSplineParameter >= 1.f || m_tInfo.fSplineParameter <= 0.f)
	{
		m_tInfo.bIsOnSpline = false;
		m_pCurrentSpline = nullptr;
		m_tInfo.bOnGround = false; // 공중 상태로 전환
		m_tInfo.bCanJump = true;   // 레일에서 떨어지면 점프 가능

		// 레일에서 떨어진 후의 상태 전환 (예: 점프/낙하)
		Change_State(ENUM_CLASS(States::MOVE), ENUM_CLASS(CSonic::SPINSTATE::SPIN_JUMP), 1.5f, false);
		return;
	}

	// 새 위치로 업데이트
	UpdatePositionOnSpline();
}

void CSonic_Controller::UpdatePositionOnSpline()
{
	if (!m_pCurrentSpline || !m_tInfo.bIsOnSpline)
		return;

	vector<CSpline_Component::SPLINE_FRAME>& splineFrames = m_pCurrentSpline->Get_SplineFrames();
	if (splineFrames.empty())
		return;

	// 스플라인 파라미터를 기반으로 정확한 위치 계산
	_float fTotalFrames = static_cast<_float>(splineFrames.size() - 1);
	_float fExactFramePos = m_tInfo.fSplineParameter * fTotalFrames;

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

	// 위치, 탄젠트, 노멀 벡터를 선형 보간하여 부드러운 값 계산
	_vector vPosition = XMVectorLerp(XMLoadFloat3(&splineFrames[uIndex1].vPosition), XMLoadFloat3(&splineFrames[uIndex2].vPosition), fLerpFactor);
	_vector vTangent = XMVectorLerp(XMLoadFloat3(&splineFrames[uIndex1].vTangent), XMLoadFloat3(&splineFrames[uIndex2].vTangent), fLerpFactor);
	_vector vNormal = XMVectorLerp(XMLoadFloat3(&splineFrames[uIndex1].vNormal), XMLoadFloat3(&splineFrames[uIndex2].vNormal), fLerpFactor);

	// 보간된 벡터 정규화
	vTangent = XMVector3Normalize(vTangent);
	vNormal = XMVector3Normalize(vNormal);
	_vector vBinormal = XMVector3Normalize(XMVector3Cross(vTangent, vNormal));

	// 레일 위 위치로 이동
	pTransform->Set_State(STATE::POSITION, vPosition);

	// 레일 진행 방향이 반대면 탄젠트 방향 반전
	if (!m_tInfo.bIsForward)
		vTangent = XMVectorNegate(vTangent);

	// 소닉의 방향 설정 (룩 벡터를 탄젠트 방향으로)
	pTransform->Set_State(STATE::LOOK, vTangent);
	pTransform->Set_State(STATE::UP, vNormal);
	pTransform->Set_State(STATE::RIGHT, vBinormal);
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