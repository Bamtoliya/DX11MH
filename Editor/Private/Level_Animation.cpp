#include "Level_Animation.h"
#include "GameInstance.h"
#include "Tools.h"

CLevel_Animation::CLevel_Animation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
    m_iLevel = { ENUM_CLASS(LEVEL::ANIMATION) };
}

HRESULT CLevel_Animation::Initialize()
{
    /*if (FAILED(m_pGameInstance->Load_GameObjects(ENUM_CLASS(LEVEL::Animation), SAVEPATH L"Terrain_Test.json")))
        return E_FAIL;*/
        /*if (FAILED(Ready_Layer_Terrain(L"Layer_Terrain")))
            return E_FAIL;
        if (FAILED(Ready_Layer_Camera(L"Layer_Camera")))
            return E_FAIL;
        if (FAILED(Ready_Layer_Sky(L"Layer_Sky")))
            return E_FAIL;
        if (FAILED(Ready_Layer_Light(L"Layer_Light")))
            return E_FAIL;
        if (FAILED(Ready_Layer_GameObjects(L"Layer_GameObjects")))
            return E_FAIL;*/
    if (FAILED(Ready_Layer_ImGui()))
        return E_FAIL;
    return S_OK;
}

void CLevel_Animation::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Animation::Render()
{
    SetWindowText(g_hWnd, TEXT("¸Ê ÅøÀÔ´Ï´Ù."));

    return S_OK;
}

HRESULT CLevel_Animation::Ready_Layer_Camera(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::ANIMATION), TEXT("Prototype_GameObject_Camera_Free"),
        m_iLevel, strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Animation::Ready_Layer_Sky(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::ANIMATION), TEXT("Prototype_GameObject_Skybox"),
        m_iLevel, strLayerTag)))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_Animation::Ready_Layer_Light(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::ANIMATION), TEXT("Prototype_GameObject_Light_Direction"),
        m_iLevel, strLayerTag)))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_Animation::Ready_Layer_ImGui()
{
    m_pGameInstance->HideAll();
    m_pGameInstance->ShowImGui(L"ToolBar");
    m_pGameInstance->ShowImGui(L"Inspector");
    m_pGameInstance->ShowImGui(L"Hierarchy");
    m_pGameInstance->ShowImGui(L"Viewport3D");
    return S_OK;
}

CLevel_Animation* CLevel_Animation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Animation* pInstance = new CLevel_Animation(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_Animation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Animation::Free()
{
    __super::Free();
}
