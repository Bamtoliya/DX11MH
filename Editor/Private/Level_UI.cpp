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

HRESULT CLevel_UI::Ready_Layer_ImGui()
{
    CBackGround::BACKGROUND_DESC				BackGroundDesc{};

    BackGroundDesc.fX = g_iWinSizeX * 0.5f;
    BackGroundDesc.fY = g_iWinSizeY * 0.5f;
    BackGroundDesc.fSizeX = 200.0f;
    BackGroundDesc.fSizeY = 200.0f;

    if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::LEVEL_UI), TEXT("Prototype_GameObject_BackGround"),
        static_cast<_uint>(LEVEL::LEVEL_UI), L"strLayerTag", &BackGroundDesc)))
        return E_FAIL;
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL::LEVEL_STATIC, "Prototype_Test", )))) return E_FAIL;
    //if (FAILED(m_pGameInstance->Add_GameObject()(L"Inspector", CInspector::Create()))) return E_FAIL;
    if (FAILED(m_pGameInstance->Add_ImGui(L"Inspector", CInspector::Create()))) return E_FAIL;
    if (FAILED(m_pGameInstance->Add_ImGui(L"Viewport2D", CViewport_2D::Create()))) return E_FAIL;
    if (FAILED(m_pGameInstance->Add_ImGui(L"Viewport3D", CViewport_3D::Create()))) return E_FAIL;
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
