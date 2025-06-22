#pragma once
#include "Ashura_Controller.h"
#include "State.h"
#include "Animator.h"

CAshura_Controller::CAshura_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CController{ pDevice, pContext }
{
}

CAshura_Controller::CAshura_Controller(const CAshura_Controller& Prototype)
	: CController{ Prototype }
{
	m_iCurStateIndex = 0;
}

HRESULT CAshura_Controller::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CAshura_Controller::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//m_pPxController = 

	return S_OK;
}
HRESULT CAshura_Controller::Late_Initialize()
{
	return S_OK;
}

CAshura_Controller* CAshura_Controller::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAshura_Controller* pInstance = new CAshura_Controller(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAshura_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CComponent* CAshura_Controller::Clone(void* pArg)
{
	CAshura_Controller* pInstance = new CAshura_Controller(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CAshura_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CAshura_Controller::Free()
{
	__super::Free();
}

HRESULT CAshura_Controller::Serialize(json& data)
{
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	return S_OK;
}
HRESULT CAshura_Controller::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

#pragma region Loops
void CAshura_Controller::Priority_Update(_float fTimeDelta)
{
}
void CAshura_Controller::Update(_float fTimeDelta)
{
	if (m_States[m_iCurStateIndex])
	{
		m_States[m_iCurStateIndex]->Execute(fTimeDelta);
		m_States[m_iCurStateIndex]->HandleInput(fTimeDelta);
	}
}
void CAshura_Controller::Late_Update(_float fTimeDelta)
{
}
HRESULT CAshura_Controller::Render()
{
	return S_OK;
}
void CAshura_Controller::Change_State(_uint iIndex)
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
void CAshura_Controller::Change_State(_uint iStateIndex, _uint iAnimIndex, _float fPlaybackSpeed, _bool bIsLoop, _float fBlendDuration)
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
HRESULT CAshura_Controller::RenderImGui()
{
	ImGui::PushID(this);
	ImGui::PopID();
	return S_OK;
}