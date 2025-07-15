#include "Level_Open.h"
#include "GameInstance.h"
#include "Transform.h"

CLevel_Open::CLevel_Open(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
	m_iLevel = { ENUM_CLASS(LEVEL::OPEN) };
}

HRESULT CLevel_Open::Initialize()
{
	if (FAILED(m_pGameInstance->Load_GameObjects(m_iLevel, SAVEPATH L"Open.json")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_GameObjects(m_iLevel, SAVEPATH L"Stage/Open/GameObjects_Open.json")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_GameObjects(m_iLevel, SAVEPATH L"Stage/Open/GameObjects_Open_Collider.json")))
		return E_FAIL;

	if (FAILED(Ready_Layer_ImGui()))
		return E_FAIL;

	//static_cast<CTransform*>(m_pGameInstance->Get_CurrentPlayer()->Get_Component(g_strTransformTag))->Set_State(STATE::POSITION, );	o

	return S_OK;
}

void CLevel_Open::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Open::Render()
{
	SetWindowText(g_hWnd, TEXT("OpenÀÔ´Ï´Ù."));
	return S_OK;
}

HRESULT CLevel_Open::Ready_Layer_ImGui()
{
	m_pGameInstance->HideAll();
	return S_OK;
}

CLevel_Open* CLevel_Open::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Open* pInstance = new CLevel_Open(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Open");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Open::Free()
{
	__super::Free();
}
