#include "Level_Stage02.h"
#include "GameInstance.h"

CLevel_Stage02::CLevel_Stage02(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
	m_iLevel = { ENUM_CLASS(LEVEL::STAGE02) };
}

HRESULT CLevel_Stage02::Initialize()
{
	if (FAILED(m_pGameInstance->Load_GameObjects(ENUM_CLASS(LEVEL::STAGE02), SAVEPATH L"Stage02.json")))
		return E_FAIL;
	return S_OK;
}

void CLevel_Stage02::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Stage02::Render()
{
	SetWindowText(g_hWnd, TEXT("Stage02ÀÔ´Ï´Ù."));
	return S_OK;
}

HRESULT CLevel_Stage02::Ready_Layer_ImGui()
{
	m_pGameInstance->HideAll();
	//m_pGameInstance->ShowImGui(L"ToolhBar");
	//m_pGameInstance->ShowImGui(L"Inspector");
	//m_pGameInstance->ShowImGui(L"Hierarchy");
	//m_pGameInstance->ShowImGui(L"Asset Browser");
	//m_pGameInstance->ShowImGui(L"Viewport3D");
	return S_OK;
}

CLevel_Stage02* CLevel_Stage02::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Stage02* pInstance = new CLevel_Stage02(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Stage02");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage02::Free()
{
	__super::Free();
}
