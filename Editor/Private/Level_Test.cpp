#include "Level_Test.h"
#include "GameInstance.h"
#include "Tools.h"

CLevel_Test::CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
    m_iLevel = { ENUM_CLASS(LEVEL::MAP) };
}

HRESULT CLevel_Test::Initialize()
{
    if (FAILED(m_pGameInstance->Load_GameObjects(ENUM_CLASS(LEVEL::MAP), SAVEPATH L"Terrain_Test2.json")))
        return E_FAIL;

    if (FAILED(Ready_Layer_ImGui()))
        return E_FAIL;
    return S_OK;
}

void CLevel_Test::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Test::Render()
{
    SetWindowText(g_hWnd, TEXT("테스트 맵 입니다."));

    return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_GameObjects(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Monster"),
        m_iLevel, strLayerTag)))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Terrain(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Terrain"),
        m_iLevel, strLayerTag)))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Camera(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Camera_Free"),
        m_iLevel, strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Sky(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Skybox"),
        m_iLevel, strLayerTag)))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Light(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Light_Direction"),
        m_iLevel, strLayerTag)))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_ImGui()
{
    m_pGameInstance->HideAll();
    m_pGameInstance->ShowImGui(L"ToolBar");
    m_pGameInstance->ShowImGui(L"Inspector");
    m_pGameInstance->ShowImGui(L"Hierarchy");
    m_pGameInstance->ShowImGui(L"Asset Browser");
    m_pGameInstance->ShowImGui(L"Viewport3D");
    return S_OK;
}

CLevel_Test* CLevel_Test::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Test* pInstance = new CLevel_Test(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_Test");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Test::Free()
{
    __super::Free();
}
