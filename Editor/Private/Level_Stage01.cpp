#include "Level_Stage01.h"
#include "GameInstance.h"

CLevel_Stage01::CLevel_Stage01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
	m_iLevel = { ENUM_CLASS(LEVEL::STAGE01) };
}

HRESULT CLevel_Stage01::Initialize()
{
	if (FAILED(m_pGameInstance->Load_GameObjects(ENUM_CLASS(LEVEL::STAGE01), SAVEPATH L"Stage01.json")))
		return E_FAIL;

	if(FAILED(Ready_Layer_ImGui()))
		return S_OK;

	return S_OK;
}

void CLevel_Stage01::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Stage01::Render()
{
	SetWindowText(g_hWnd, TEXT("STAGE01ÀÔ´Ï´Ù."));
	return S_OK;
}

HRESULT CLevel_Stage01::Ready_Layer_ImGui()
{
	m_pGameInstance->HideAll();
	//m_pGameInstance->ShowImGui(L"ToolBar");
	//m_pGameInstance->ShowImGui(L"Inspector");
	//m_pGameInstance->ShowImGui(L"Hierarchy");
	//m_pGameInstance->ShowImGui(L"Asset Browser");
	//m_pGameInstance->ShowImGui(L"Viewport3D");
	return S_OK;
}

CLevel_Stage01* CLevel_Stage01::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Stage01* pInstance = new CLevel_Stage01(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Stage01");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage01::Free()
{
	__super::Free();
}
