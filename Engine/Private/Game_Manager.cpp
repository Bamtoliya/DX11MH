#include "Game_Manager.h"
#include "GameInstance.h"

#pragma region Constructor&Desctructor
CGame_Manager::CGame_Manager()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
	, m_pCurrentPlayer{ nullptr }
{
	Safe_AddRef(m_pGameInstance);
}

void CGame_Manager::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
HRESULT CGame_Manager::Initialize()
{
	// 초기화 로직이 필요하다면 여기에 작성
	return S_OK;
}

CGame_Manager* CGame_Manager::Create()
{
	return new CGame_Manager();
}
#pragma endregion




void CGame_Manager::Culling_Collider(_float fDistance)
{

}


