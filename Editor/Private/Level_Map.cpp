#include "Level_Map.h"

CLevel_Map::CLevel_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Map::Initialize()
{
    if (FAILED(Ready_Layer_ImGui()))
        return E_FAIL;
    return S_OK;
}

void CLevel_Map::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Map::Render()
{
    SetWindowText(g_hWnd, TEXT("¸Ê Åø ÀÔ´Ï´Ù."));

    return S_OK;
}

HRESULT CLevel_Map::Ready_Layer_ImGui()
{
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
