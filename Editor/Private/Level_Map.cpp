#include "Level_Map.h"
#include "GameInstance.h"
#include "Tools.h"

CLevel_Map::CLevel_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Map::Initialize()
{
    m_iLevel = ENUM_CLASS(LEVEL::LEVEL_MAP);
    if (FAILED(Ready_Layer_Camera(L"Layer_Camera")))
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
    return S_OK;
}

HRESULT CLevel_Map::Ready_Layer_Terrain(const _wstring& strLayerTag)
{
    return S_OK;
}

HRESULT CLevel_Map::Ready_Layer_Camera(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::LEVEL_UI), TEXT("Prototype_GameObject_Camera_Free"),
        m_iLevel, strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Map::Ready_Layer_ImGui()
{
    m_pGameInstance->Add_ImGui(L"Inspector", CInspector::Create(m_pDevice, m_pContext));
    m_pGameInstance->Add_ImGui(L"Hierarchy", CHierarchy::Create(m_pDevice, m_pContext));
    m_pGameInstance->Add_ImGui(L"Asset Browser", CAsset_Browser::Create(m_pDevice, m_pContext));
    m_pGameInstance->Add_ImGui(L"Viewport3D", CViewport_3D::Create(m_pDevice, m_pContext));
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
