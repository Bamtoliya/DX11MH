#include "Level_UI.h"

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
    SetWindowText(g_hWnd, TEXT("로고레벨입니다."));

    return S_OK;
}

HRESULT CLevel_UI::Ready_Layer_ImGui()
{
    return S_OK;
}

CLevel_UI* CLevel_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_UI* pInstance = new CLevel_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_Logo");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_UI::Free()
{
    __super::Free();
}
