#include "Level_Map.h"
#include "GameInstance.h"
#include "Tools.h"

CLevel_Map::CLevel_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Map::Initialize()
{
    m_iLevel = ENUM_CLASS(LEVEL::MAP);
    if (FAILED(Ready_Layer_Terrain(L"Layer_Terrain")))
        return E_FAIL;
    if (FAILED(Ready_Layer_Camera(L"Layer_Camera")))
        return E_FAIL;
    if (FAILED(Ready_Layer_Sky(L"Layer_Sky")))
        return E_FAIL;
    if (FAILED(Ready_Layer_GameObjects(L"Layer_GameObjects")))
        return E_FAIL;
    if (FAILED(Ready_Layer_ImGui()))
        return E_FAIL;
    return S_OK;
}

void CLevel_Map::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Map::Render()
{
    SetWindowText(g_hWnd, TEXT("맵 툴입니다."));

    return S_OK;
}

HRESULT CLevel_Map::Ready_Layer_GameObjects(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Monster"),
        m_iLevel, strLayerTag)))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_Map::Ready_Layer_Terrain(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Terrain"),
        m_iLevel, strLayerTag)))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_Map::Ready_Layer_Camera(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Camera_Free"),
        m_iLevel, strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Map::Ready_Layer_Sky(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Skybox"),
        m_iLevel, strLayerTag)))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_Map::Ready_Layer_ImGui()
{
    m_pGameInstance->HideAll();
    m_pGameInstance->ShowImGui(L"ToolBar");
    m_pGameInstance->ShowImGui(L"Inspector");
    m_pGameInstance->ShowImGui(L"Hierarchy");
    m_pGameInstance->ShowImGui(L"Asset Browser");
    m_pGameInstance->ShowImGui(L"Viewport3D");
    return S_OK;
}

CLevel_Map* CLevel_Map::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Map* pInstance = new CLevel_Map(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_Map");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Map::Free()
{
    __super::Free();
}
