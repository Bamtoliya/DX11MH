#include "Inspector.h"
CInspector::CInspector()
{
}

HRESULT CInspector::Initialize()
{
    m_bShow = true;
	return S_OK;
}

void CInspector::Update(_float fTimeDelta)
{

}

HRESULT CInspector::Render()
{
    if (m_bShow)
    {
        ImGui::Begin("Inspector");
        ImGui::Text("This can dock into the fullscreen space.");
        ImGui::End();
    }

    return S_OK;
}

CInspector* CInspector::Create()
{
    CInspector* pInstance = new CInspector();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CInspector");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInspector::Free()
{
    __super::Free();
}
