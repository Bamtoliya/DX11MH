#include "Inspector.h"
#include "GameObject.h"
CInspector::CInspector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMyImGui { m_pDevice, m_pContext }
{
}

HRESULT CInspector::Initialize(void* pArg)
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
        if (m_pGameObject)
            m_pGameObject->RenderImGui();
        ImGui::End();
    }

    return S_OK;
}

CInspector* CInspector::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CInspector* pInstance = new CInspector(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
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
