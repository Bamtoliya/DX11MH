#include "Level_Stage01.h"
#include "GameInstance.h"

CLevel_Stage01::CLevel_Stage01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
	m_iLevel = { ENUM_CLASS(LEVEL::STAGE01) };
}

HRESULT CLevel_Stage01::Initialize()
{
	if (FAILED(m_pGameInstance->Load_GameObjects(m_iLevel, SAVEPATH L"Stage01.json")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_GameObjects(m_iLevel, SAVEPATH L"Stage/W6D01/GameObjects_W6D01.json")))
   		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_GameObjects(m_iLevel, SAVEPATH L"Stage/W6D01/W6D01StageObjects.json")))
		return E_FAIL;


	if(FAILED(Ready_Layer_ImGui()))
		return E_FAIL;

	CShadow::SHADOW_DESC		Desc{};
	Desc.vEye = _float4(640.f, 180.f, 375.f, 1);
	Desc.vAt = _float4(600.f, 150.f, 400.f, 1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 500.f;

	if (FAILED(m_pGameInstance->Ready_Light_For_Shadow(Desc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_GameObjects(m_iLevel, SAVEPATH L"UI/UI_BottomRight.json")))
		return E_FAIL;

	static_cast<CTransform*>(m_pGameInstance->Get_CurrentPlayer()->Get_Component(g_strTransformTag))->Set_State(STATE::POSITION, XMVectorSet(600.f, 160.f, 380.f, 1.f));
	m_pGameInstance->Set_Camera(1);

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
