#include "Sonic_State.h"

using namespace Editor;

HRESULT CSonic_State::Initialize()
{
    return S_OK;
}

void CSonic_State::Enter()
{

}

void CSonic_State::Execute(_float fTimeDelta)
{

}

void CSonic_State::HandleInput(_float fTimeDelta)
{

}

void CSonic_State::Exit()
{
    __super::Free();
}
