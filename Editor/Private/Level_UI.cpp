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
    BackGroundDesc.fX = g_iWinSizeX * 0.5f;
    BackGroundDesc.fY = g_iWinSizeY * 0.5f;
    BackGroundDesc.fSizeX = 200.0f;
    BackGroundDesc.fSizeY = 200.0f;

    if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::LEVEL_UI), TEXT("Prototype_GameObject_BackGround"),
        ENUM_CLASS(LEVEL::LEVEL_UI), strLayerTag, &BackGroundDesc)))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_UI::Ready_Layer_Camera(const wstring strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::LEVEL_UI), TEXT("Prototype_GameObject_Camera_Free"),
        ENUM_CLASS(LEVEL::LEVEL_UI), strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_UI::Ready_Layer_ImGui()
{
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL::LEVEL_STATIC, "Prototype_Test", )))) return E_FAIL;
    //if (FAILED(m_pGameInstance->Add_GameObject()(L"Inspector", CInspector::Create()))) return E_FAIL;
    if (FAILED(m_pGameInstance->Add_ImGui(L"Inspector", CInspector::Create(m_pDevice, m_pContext)))) return E_FAIL;
    if (FAILED(m_pGameInstance->Add_ImGui(L"Hierarchy", CHierarchy::Create(m_pDevice, m_pContext)))) return E_FAIL;
    if (FAILED(m_pGameInstance->Add_ImGui(L"Viewport2D", CViewport_2D::Create(m_pDevice, m_pContext)))) return E_FAIL;
    if (FAILED(m_pGameInstance->Add_ImGui(L"Viewport3D", CViewport_3D::Create(m_pDevice, m_pContext)))) return E_FAIL;
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
