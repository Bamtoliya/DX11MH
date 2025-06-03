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
	m_iCurStateIndex = ENUM_CLASS(CSonic::STATE::IDLE_LOOP);
}

HRESULT CSonic_Controller::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CSonic_Controller::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

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
	if (m_States[m_iCurStateIndex])
	{
		m_States[m_iCurStateIndex]->HandleInput();
		m_States[m_iCurStateIndex]->Execute(fTimeDelta);
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
		static_cast<CSonic_Ground_State*>(m_States[iIndex])->Set_Parameter(static_cast<CSonic_Ground_State*>(m_States[m_iCurStateIndex])->Get_Parameter());
		m_iCurStateIndex = iIndex;
		CAnimator* pAnimator = static_cast<CAnimator*>(m_pOwner->Get_Component(L"Com_Animator"));
		if (pAnimator)
		{
			_bool bLoop = m_States[m_iCurStateIndex]->Get_IsLoop();
			_float fPlaybackSpeed = 1.0f;
			_float fBlendDuration = m_States[m_iCurStateIndex]->Get_BlendDuration();

			pAnimator->Change_Animation(m_iCurStateIndex, bLoop, fPlaybackSpeed, fBlendDuration);
		}
	}
}
#pragma endregion
HRESULT CSonic_Controller::RenderImGui()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Sonic Controller", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if(m_States[m_iCurStateIndex])
			m_States[m_iCurStateIndex]->RenderImGui();
	}
	ImGui::PopID();
	return S_OK;
}