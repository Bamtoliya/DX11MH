#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "Tools.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
    m_iLevel = { ENUM_CLASS(LEVEL::GAMEPLAY) };
}

HRESULT CLevel_GamePlay::Initialize()
{
    if (FAILED(m_pGameInstance->Load_GameObjects(ENUM_CLASS(LEVEL::GAMEPLAY), SAVEPATH L"GamePlay_Test.json")))
        return E_FAIL;

    /*if (FAILED(Ready_Layer_Camera(L"Layer_Camera")))
        return E_FAIL;
    if (FAILED(Ready_Layer_Sky(L"Layer_Sky")))
        return E_FAIL;
    if (FAILED(Ready_Layer_GameObjects(L"Layer_GameObjects")))
        return E_FAIL;
    if (FAILED(Ready_Layer_ImGui()))
        return E_FAIL;*/
    return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
}

HRESULT CLevel_GamePlay::Render()
{
    SetWindowText(g_hWnd, TEXT("애니메이션 레벨입니다."));

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_ImGui()
{
    m_pGameInstance->HideAll();
    m_pGameInstance->ShowImGui(L"ToolBar");
    m_pGameInstance->ShowImGui(L"Inspector");
    m_pGameInstance->ShowImGui(L"Hierarchy");
    m_pGameInstance->ShowImGui(L"Viewport3D");
    return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_GamePlay");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_GamePlay::Free()
{
    __super::Free();
}
