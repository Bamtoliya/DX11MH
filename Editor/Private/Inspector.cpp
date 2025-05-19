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
}

HRESULT CInspector::Render()
{
    if (m_bShow)
    {
        if (ImGui::Begin("Inspector", &m_bShow))
        {
            CGameObject* pGameObject = m_pGameInstance->Get_PickedObject();
            if (pGameObject)
            {
                string tmp = WideToUtf8(pGameObject->Get_Name().c_str());
                char szBuffer[MAX_PATH] = {};
                strncpy_s(szBuffer, MAX_PATH, tmp.c_str(), _TRUNCATE);
                //strncpy_s(szBuffer, tmp.c_str());
                ImGui::InputText("##ObjectName:", szBuffer, MAX_PATH);
                pGameObject->Set_Name(Utf8ToWide(szBuffer));
                ImGui::Text("Object ID: %d", pGameObject->Get_ID());
            }
            else
            {
                ImGui::Text("Intentionaly Blank");
            }

            if (pGameObject)
            {
                pGameObject->RenderImGuiComponents();
                if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                {
                    ImGui::OpenPopup("my popup");
                }
                PopUp_Components();
                Add_Component_Button();
                Add_Component_PopUp();
            }

            ImGui::End();
        }
    }
    return S_OK;
}

static int MyCallback(ImGuiInputTextCallbackData* data)
{
    if (data->EventFlag == ImGuiInputTextFlags_CallbackCompletion)
    {
        data->InsertChars(data->CursorPos, "Com_");
    }
    return 0;
}

HRESULT CInspector::Add_Component_PopUp()
{
    static _wstring strCurSelPrototype = {};
    
    if (ImGui::BeginPopup("Add Component", ImGuiWindowFlags_AlwaysAutoResize)) {
        for (_uint i = 0; i < ENUM_CLASS(LEVEL::END); ++i)
        {
            ImGui::SameLine();
            if (ImGui::Button(ToString(i)))
            {
                m_eCurSelLevel = ENUM_LEVEL(i);
            }
        }
        ImGui::Separator();

        if (ImGui::BeginListBox(ToString(m_eCurSelLevel)))
        {
            m_pGameInstance->Render_PrototypeManagerImGui(ENUM_CLASS(m_eCurSelLevel), L"Component", strCurSelPrototype);
            ImGui::EndListBox();
        }
        ImGui::Separator();

        ImGui::Spacing(); // 약간의 공백 추가

        static wstring strComponentTag = {};

        /*if (ImGui::InputTextWithHint("##Layer_Text_Input: ", "Layer_", szBuffer, iBufferSize, ImGuiInputTextFlags_CallbackCompletion, MyCallback))
        {

        }*/

        if (ImGui::Button("Add", ImVec2(120, 0)))
        {
            if (FAILED(m_pGameInstance->Get_PickedObject()->Add_Component(ENUM_CLASS(m_eCurSelLevel), strCurSelPrototype,
                strComponentTag, nullptr, nullptr)))
            {
                return E_FAIL;
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus(); // '닫기' 버튼에 기본 포커스

        ImGui::EndPopup();
    }
    return S_OK;
}

HRESULT CInspector::PopUp_Components()
{
    if (ImGui::BeginPopupContextItem("my popup"))
    {
        Add_Component_Button();
        if (ImGui::Button("Delete Component"))
        {

        }
        ImGui::EndPopup();
    }
    return S_OK;
}

HRESULT CInspector::Add_Component_Button()
{
    if (ImGui::Button("Add Component"))
    {
        ImGui::OpenPopup("Add Component");
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
