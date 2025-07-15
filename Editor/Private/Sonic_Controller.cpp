#include "Sonic_Controller.h"
#include "Sonic.h"
#include "State.h"
#include "Sonic_States.h"
#include "Animator.h"

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
	//Change_State(SONIC_STATE::RAIL);
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
