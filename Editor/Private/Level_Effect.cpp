#include "Level_Effect.h"

#include "Level_Effect.h"

CLevel_Effect::CLevel_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Effect::Initialize()
{
    if (FAILED(Ready_Layer_ImGui()))
        return E_FAIL;
    return S_OK;
}

void CLevel_Effect::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Effect::Render()
{
    SetWindowText(g_hWnd, TEXT("이펙트 툴 레벨입니다."));

    return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer_ImGui()
{
    return S_OK;
}

CLevel_Effect* CLevel_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Effect* pInstance = new CLevel_Effect(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_Effect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Effect::Free()
{
    __super::Free();
}

