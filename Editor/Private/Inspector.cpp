#include "Inspector.h"
#include "GameObject.h"
#include "GameInstance.h"
CInspector::CInspector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMyImGui { pDevice, pContext }
{
}

HRESULT CInspector::Initialize(void* pArg)
{
    m_bShow = true;
	return S_OK;
}

void CInspector::Update(_float fTimeDelta)
{
    m_pGameObject = m_pGameInstance->Get_PickedObject();
}

HRESULT CInspector::Render()
{
    if (ImGui::Begin("Inspector", &m_bShow))
    {
        if (m_pGameObject)
        {
            string tmp = WideToUtf8(m_pGameObject->Get_Name().c_str());
            ImGui::Text(tmp.c_str());
        }
        else
        {
            ImGui::Text("Intentionaly Blank");
        }

        if (m_pGameObject)
        {
            m_pGameObject->RenderImGuiComponents();
            if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                ImGui::OpenPopup("my popup");
            }
            PopUpComponents();
        }
            
        ImGui::End();
    }
    return S_OK;
}

HRESULT CInspector::PopUpComponents()
{
    if (ImGui::BeginPopupContextItem("my popup"))
    {
        if (ImGui::Button("Add Component"));
        if (ImGui::Button("Delete Component"));
        ImGui::EndPopup();
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
