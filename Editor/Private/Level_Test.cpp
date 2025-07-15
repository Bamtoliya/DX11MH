#include "Level_Test.h"
#include "GameInstance.h"
#include "Tools.h"

CLevel_Test::CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
    m_iLevel = { ENUM_CLASS(LEVEL::TEST) };
}

HRESULT CLevel_Test::Initialize()
{

    if (FAILED(m_pGameInstance->Load_GameObjects(m_iLevel, SAVEPATH L"Test_Field.json")))
        return E_FAIL;

    /*Ready_Layer_GameObjects(L"Layer_GameObjects");
    Ready_Layer_Light(L"Layer_Light");
    Ready_Layer_Terrain(L"Layer_Virtual_Space");
    Ready_Layer_Camera(L"Layer_Camera");*/

    CShadow::SHADOW_DESC		Desc{};
    Desc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
    Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
    Desc.fFovy = XMConvertToRadians(60.0f);
    Desc.fNear = 0.1f;
    Desc.fFar = 500.f;

    if (FAILED(m_pGameInstance->Ready_Light_For_Shadow(Desc)))
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
    if (FAILED(m_pGameInstance->Add_GameObject(m_iLevel, TEXT("Prototype_GameObject_Sonic"),
        m_iLevel, strLayerTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject(m_iLevel, TEXT("Prototype_GameObject_Tutorial_Enemy"),
        m_iLevel, strLayerTag)))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Terrain(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(m_iLevel, TEXT("Prototype_GameObject_Virtual_Space"),
        m_iLevel, strLayerTag)))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Camera(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(m_iLevel, TEXT("Prototype_GameObject_Camera_Free"),
        m_iLevel, strLayerTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject(m_iLevel, TEXT("Prototype_GameObject_Camera_Follow"),
        m_iLevel, strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Sky(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(m_iLevel, TEXT("Prototype_GameObject_Skybox"),
        m_iLevel, strLayerTag)))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Light(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(m_iLevel, TEXT("Prototype_GameObject_Light_Direction"),
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
