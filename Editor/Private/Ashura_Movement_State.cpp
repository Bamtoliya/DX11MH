#include "Ashura_Movement_State.h"
#include "Ashura.h"
#include "Animator.h"
#include "Ashura_Controller.h"

using namespace Editor;

HRESULT CAshura_Movement_State::Initialize()
{
    m_bLoop = true;
    //m_iCurState = ENUM_CLASS(CSonic_Controller::States::COMBAT);
    return S_OK;
}

CAshura_Movement_State* CAshura_Movement_State::Create(CGameObject* pOwner)
{
    CAshura_Movement_State* pInstance = new CAshura_Movement_State(pOwner);
    pInstance->Initialize();
    return pInstance;
}

void CAshura_Movement_State::Enter()
{
}

void CAshura_Movement_State::Execute(_float fTimeDelta)
{
    CAshura_Controller* pController = static_cast<CAshura_Controller*>(m_pOwner->Get_Component(L"Com_Controller"));
    pController->Change_State(0, ENUM_CLASS(CAshura::STATE::SLAM_ATK_C_START));
}
void CAshura_Movement_State::HandleInput(_float fTimeDelta)
{
}

void CAshura_Movement_State::Exit()
{
}