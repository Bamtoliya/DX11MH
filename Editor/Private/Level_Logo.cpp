#include "Level_Logo.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "Level_Open.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
		: CLevel { pDevice, pContext }
{
	m_iLevel = { ENUM_CLASS(LEVEL::LOGO) };
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(m_pGameInstance->Load_GameObjects(m_iLevel, SAVEPATH L"Logo.json")))
		return E_FAIL;

	m_pGameInstance->HideAll();
	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Change_Level(ENUM_CLASS(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::OPEN))))
			return;
	}

}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));

	return S_OK;
}


CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Logo::Free()
{
	__super::Free();

}
