#include "Asset_Browser.h"

CAsset_Browser::CAsset_Browser(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMyImGui{ pDevice, pContext }
{
}

CAsset_Browser* CAsset_Browser::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CAsset_Browser* pInstance = new CAsset_Browser(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CAsset_Browser");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAsset_Browser::Free()
{
    __super::Free();
}

HRESULT CAsset_Browser::Initialize(void* pArg)
{
    m_bShow = true;
	return S_OK;
}

void CAsset_Browser::Update(_float fTimeDelta)
{
}

HRESULT CAsset_Browser::Render()
{
	return S_OK;
}
