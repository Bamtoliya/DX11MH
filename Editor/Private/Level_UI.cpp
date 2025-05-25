#include "Level_UI.h"
#include "GameInstance.h"
#include "Tools.h"
#include "BackGround.h"

CLevel_UI::CLevel_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel {pDevice, pContext}
{
}

HRESULT CLevel_UI::Initialize()
{
    m_iLevel = ENUM_CLASS(LEVEL::UI);
    if(FAILED(Ready_Layer_ImGui()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Background(L"Layer_Background")))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(L"Layer_Camera")))
        return E_FAIL;
    return S_OK;
}

void CLevel_UI::Update(_float fTimeDelta)
{
}

HRESULT CLevel_UI::Render()
{
    SetWindowText(g_hWnd, TEXT("UI 툴 입니다."));

    return S_OK;
}

HRESULT CLevel_UI::Ready_Layer_Background(const wstring strLayerTag)
{
    CBackGround::BACKGROUND_DESC				BackGroundDesc{};

    lstrcpy(BackGroundDesc.szName, L"Background");
    BackGroundDesc.vPos.x = g_iWinSizeX * 0.5f;
    BackGroundDesc.vPos.y = g_iWinSizeY * 0.5f;
    BackGroundDesc.vSize.x = 200.0f;
    BackGroundDesc.vSize.y = 200.0f;

    if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_GameObject_BackGround"),
        m_iLevel, strLayerTag, &BackGroundDesc)))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_UI::Ready_Layer_Camera(const wstring strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_GameObject_Camera_Free"),
        m_iLevel, strLayerTag)))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_UI::Ready_Layer_ImGui()
{
    m_pGameInstance->HideAll();
    m_pGameInstance->ShowImGui(L"ToolBar");
    m_pGameInstance->ShowImGui(L"Inspector");
    m_pGameInstance->ShowImGui(L"Hierarchy");
    m_pGameInstance->ShowImGui(L"Browser");
    m_pGameInstance->ShowImGui(L"Viewport2D");
    return S_OK;
}

CLevel_UI* CLevel_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_UI* pInstance = new CLevel_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_UI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_UI::Free()
{
    __super::Free();
}
