#include "Sonic_Combat_State.h"
#include "GameInstance.h"

#include "Sonic.h"
#include "Sonic_Controller.h"
#include "Animator.h"


using namespace Editor;

HRESULT CSonic_Combat_State::Initialize()
{
    m_bLoop = false;
    return S_OK;
}

void CSonic_Combat_State::Idle()
{
    CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
    if (!pController) return;
    CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
    if (!status.bComboInputBuffered)
    {
        CAnimator* pAnimator = static_cast<CAnimator*>(m_pOwner->Get_Component(L"Com_Animator"));
        if (pAnimator->Get_AnimationProgress() >= 0.6f)
        {
            pController->ClearComboInputBuffer(); // IDLE로 갈 때 버퍼 클리어
            status.fMoveSpeed.first = 0.f;
            XMStoreFloat3(&status.vMoveDirection, XMVectorZero());
            pController->Change_State(CSonic::STATE::IDLE_LOOP);
        }
    }
    else if (status.bComboInputBuffered && !ANY_KEY)
    {
        // 선입력은 있지만 현재 아무키도 안눌려있으면, 선입력 타이머에 의해 해제될 때까지 대기
        // 또는 즉시 IDLE로 갈 수도 있음 (게임 디자인에 따라 결정)
    }
}

void CSonic_Combat_State::Execute(_float fTimeDelta)
{

}

void CSonic_Combat_State::HandleInput(_float fTimeDelta)
{
    if (KEY_PRESSING(DIK_I))
    {
        CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
        pController->BufferComboInput();
    }
    Idle();
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
    CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
    CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
    CAnimator* pAnimator = static_cast<CAnimator*>(m_pOwner->Get_Component(L"Com_Animator"));
    _float progress = pAnimator->Get_AnimationProgress();
    if (status.bComboInputBuffered && progress >= 0.6f)
    {
        pController->Change_State(CSonic::STATE::COMBO_KICK02);
        return;
    }
    __super::Idle();
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
    CSonic_Controller* pController = static_cast<CSonic_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
    CSonic_Controller::SONIC_STATUS& status = pController->Get_Status();
    CAnimator* pAnimator = static_cast<CAnimator*>(m_pOwner->Get_Component(L"Com_Animator"));
    _float progress = pAnimator->Get_AnimationProgress();
    if (status.bComboInputBuffered && progress >= 0.6f)
    {
        pController->Change_State(CSonic::STATE::COMBO_ONETWO);
        return;
    }
    __super::Idle();
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

