#include "Sonic_Combat_State.h"
#include "GameInstance.h"

#include "Sonic.h"
#include "Sonic_Controller.h"
#include "Animator.h"
#include "Sound_Event.h"


using namespace Editor;

HRESULT CSonic_Combat_State::Initialize()
{
    m_bLoop = false;
	m_iCurState = ENUM_CLASS(CSonic_Controller::States::COMBAT);
    return S_OK;
}

CSonic_Combat_State* CSonic_Combat_State::Create(CGameObject* pOwner)
{
    CSonic_Combat_State* pInstance = new CSonic_Combat_State(pOwner);
    pInstance->Initialize();
    return pInstance;
}

void CSonic_Combat_State::Execute(_float fTimeDelta)
{
    static_cast<CSonic*>(m_pOwner)->Change_ModelIndex(0);
    CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
	CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
    CAnimator* pAnimator = static_cast<CAnimator*>(m_pOwner->Get_Component(L"Com_Animator"));
    _float progress = pAnimator->Get_AnimationProgress();
    if (status.bComboInputBuffered && progress >= 0.6f)
    {
        pController->Change_State(m_iCurState, ENUM_CLASS(CSonic::STATE::COMBO_KICK01) + status.iCombos.first++, 1.5f, m_bLoop);
        if(status.iCombos.first >= status.iCombos.second)
        {
            status.iCombos.first = 0; // 콤보가 끝나면 초기화
		}
        return;
    }

    if (!status.bComboInputBuffered && progress >= 0.6f)
    {
        pController->Change_State(ENUM_CLASS(CSonic_Controller::States::MOVE), ENUM_CLASS(CSonic::STATE::IDLE01_LOOP), 1.5f, m_bLoop); // 콤보 입력이 없으면 이동 상태로 전환
    }
}

void CSonic_Combat_State::HandleInput(_float fTimeDelta)
{
    if (KEY_PRESSING(DIK_I))
    {
        CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
        pController->BufferComboInput();
    }
}

CSonic_Combo_Kick01_State* CSonic_Combo_Kick01_State::Create(CGameObject* pOwner)
{
    CSonic_Combo_Kick01_State* pInstance = new CSonic_Combo_Kick01_State(pOwner);
    pInstance->Initialize();
    return pInstance;
}


HRESULT CSonic_Combo_Kick01_State::Initialize()
{
    strcpy_s(m_szName, "Sonic_Combo_Kick01_State");
    return S_OK;
}

void CSonic_Combo_Kick01_State::Enter()
{
    CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
    if (pController)
        pController->ClearComboInputBuffer(); // 새 콤보 시작 시 이전 선입력 클리어
}

void CSonic_Combo_Kick01_State::Execute(_float fTimeDelta)
{
    static_cast<CSonic*>(m_pOwner)->Change_ModelIndex(0);
    CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
    CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
    CAnimator* pAnimator = static_cast<CAnimator*>(m_pOwner->Get_Component(L"Com_Animator"));
    _float progress = pAnimator->Get_AnimationProgress();
    if (status.bComboInputBuffered && progress >= 0.6f)
    {
        pController->Change_State(CSonic::STATE::COMBO_KICK02);
        return;
    }
}

void CSonic_Combo_Kick01_State::HandleInput(_float fTimeDelta)
{
    __super::HandleInput(fTimeDelta);
}

void CSonic_Combo_Kick01_State::Exit()
{

}


CSonic_Combo_Kick02_State* CSonic_Combo_Kick02_State::Create(CGameObject* pOwner)
{
    CSonic_Combo_Kick02_State* pInstance = new CSonic_Combo_Kick02_State(pOwner);
    pInstance->Initialize();
    return pInstance;
}


HRESULT CSonic_Combo_Kick02_State::Initialize()
{
    strcpy_s(m_szName, "Sonic_Combo_Kick02_State");
    return S_OK;
}

void CSonic_Combo_Kick02_State::Enter()
{
    CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
    if (pController)
        pController->ClearComboInputBuffer(); // 새 콤보 시작 시 이전 선입력 클리어
}

void CSonic_Combo_Kick02_State::Execute(_float fTimeDelta)
{
    static_cast<CSonic*>(m_pOwner)->Change_ModelIndex(0);
    CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
    CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
    CAnimator* pAnimator = static_cast<CAnimator*>(m_pOwner->Get_Component(L"Com_Animator"));
    _float progress = pAnimator->Get_AnimationProgress();
    if (status.bComboInputBuffered && progress >= 0.6f)
    {
        pController->Change_State(CSonic::STATE::COMBO_ONETWO);
        return;
    }
}

void CSonic_Combo_Kick02_State::HandleInput(_float fTimeDelta)
{
    __super::HandleInput(fTimeDelta);
}

void CSonic_Combo_Kick02_State::Exit()
{

}


CSonic_Combo_OneTwo_State* CSonic_Combo_OneTwo_State::Create(CGameObject* pOwner)
{
    CSonic_Combo_OneTwo_State* pInstance = new CSonic_Combo_OneTwo_State(pOwner);
    pInstance->Initialize();
    return pInstance;
}


HRESULT CSonic_Combo_OneTwo_State::Initialize()
{
    strcpy_s(m_szName, "Sonic_Combo_OneTwo_State");
    return S_OK;
}

void CSonic_Combo_OneTwo_State::Enter()
{
    CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
    if (pController)
        pController->ClearComboInputBuffer(); // 새 콤보 시작 시 이전 선입력 클리어
}

void CSonic_Combo_OneTwo_State::Execute(_float fTimeDelta)
{
    static_cast<CSonic*>(m_pOwner)->Change_ModelIndex(0);
    CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
    CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
    CAnimator* pAnimator = static_cast<CAnimator*>(m_pOwner->Get_Component(L"Com_Animator"));
    _float progress = pAnimator->Get_AnimationProgress();
    if (status.bComboInputBuffered && progress >= 0.6f)
    {
        pController->Change_State(CSonic::STATE::COMBO_PUNCH01);
        return;
    }
}

void CSonic_Combo_OneTwo_State::HandleInput(_float fTimeDelta)
{
    __super::HandleInput(fTimeDelta);
}

void CSonic_Combo_OneTwo_State::Exit()
{

}


CSonic_Combo_Punch01_State* CSonic_Combo_Punch01_State::Create(CGameObject* pOwner)
{
    CSonic_Combo_Punch01_State* pInstance = new CSonic_Combo_Punch01_State(pOwner);
    pInstance->Initialize();
    return pInstance;
}


HRESULT CSonic_Combo_Punch01_State::Initialize()
{
    strcpy_s(m_szName, "Sonic_Combo_Punch01_State");
    return S_OK;
}

void CSonic_Combo_Punch01_State::Enter()
{
    CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
    if (pController)
        pController->ClearComboInputBuffer(); // 새 콤보 시작 시 이전 선입력 클리어
}

void CSonic_Combo_Punch01_State::Execute(_float fTimeDelta)
{
    static_cast<CSonic*>(m_pOwner)->Change_ModelIndex(0);
    CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
    CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
    CAnimator* pAnimator = static_cast<CAnimator*>(m_pOwner->Get_Component(L"Com_Animator"));
    _float progress = pAnimator->Get_AnimationProgress();
    if (status.bComboInputBuffered && progress >= 0.6f)
    {
        pController->Change_State(CSonic::STATE::COMBO_PUNCH02);
        return;
    }
}

void CSonic_Combo_Punch01_State::HandleInput(_float fTimeDelta)
{
    __super::HandleInput(fTimeDelta);
}

void CSonic_Combo_Punch01_State::Exit()
{

}


CSonic_Combo_Punch02_State* CSonic_Combo_Punch02_State::Create(CGameObject* pOwner)
{
    CSonic_Combo_Punch02_State* pInstance = new CSonic_Combo_Punch02_State(pOwner);
    pInstance->Initialize();
    return pInstance;
}


HRESULT CSonic_Combo_Punch02_State::Initialize()
{
    strcpy_s(m_szName, "Sonic_Combo_Punch02_State");
    return S_OK;
}

void CSonic_Combo_Punch02_State::Enter()
{
    CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
    if (pController)
        pController->ClearComboInputBuffer(); // 새 콤보 시작 시 이전 선입력 클리어
}

void CSonic_Combo_Punch02_State::Execute(_float fTimeDelta)
{
    static_cast<CSonic*>(m_pOwner)->Change_ModelIndex(0);
    CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
    CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
    CAnimator* pAnimator = static_cast<CAnimator*>(m_pOwner->Get_Component(L"Com_Animator"));
    _float progress = pAnimator->Get_AnimationProgress();
    if (status.bComboInputBuffered && progress >= 0.6f)
    {
        pController->Change_State(CSonic::STATE::COMBO_KICK01);
        return;
    }
}

void CSonic_Combo_Punch02_State::HandleInput(_float fTimeDelta)
{
    __super::HandleInput(fTimeDelta);
}

void CSonic_Combo_Punch02_State::Exit()
{

}

