#pragma once
#include "State.h"
#include "GameInstance.h"

CState::CState(CGameObject* pOwner)
	: m_pOwner { pOwner }
	, m_pGameInstance{CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pOwner);
	Safe_AddRef(m_pGameInstance);
}


void CState::Free()
{
	Safe_Release(m_pOwner);
	Safe_Release(m_pGameInstance);
}