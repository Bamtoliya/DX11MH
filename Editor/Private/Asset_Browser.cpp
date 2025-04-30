#include "Asset_Browser.h"

CAsset_Browser::CAsset_Browser()
{
}

CAsset_Browser* CAsset_Browser::Create()
{
    CAsset_Browser* pInstance = new CAsset_Browser();

    if (FAILED(pInstance->Initialize()))
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

HRESULT CAsset_Browser::Initialize()
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
