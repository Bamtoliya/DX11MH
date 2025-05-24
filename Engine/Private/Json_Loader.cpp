#include "Json_Loader.h"
#include "GameInstance.h"

CJson_Loader::CJson_Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

CJson_Loader* CJson_Loader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CJson_Loader* pInstance = new CJson_Loader(pDevice, pContext);
	if (nullptr == pInstance)
		return nullptr;
	return pInstance;
}

void CJson_Loader::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}