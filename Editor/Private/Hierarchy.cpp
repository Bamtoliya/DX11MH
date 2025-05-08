#include "Hierarchy.h"

CHierarchy::CHierarchy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMyImGui {pDevice, pContext}
{
}

CHierarchy* CHierarchy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CHierarchy* pInstance = new CHierarchy(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CHierarchy");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHierarchy::Free()
{
    __super::Free();
}

HRESULT CHierarchy::Initialize(void* pArg)
{
    m_bShow = true;
    return S_OK;
}

void CHierarchy::Priority_Update(_float fTimeDelta)
{
}

void CHierarchy::Update(_float fTimeDelta)
{
}

void CHierarchy::Late_Update(_float fTimeDelta)
{
}

HRESULT CHierarchy::Render()
{
    return S_OK;
}
