#include "Hierarchy.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "UIObject.h"

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
    if (!m_bShow)
        return S_OK;

    ImGui::Begin("Hierarchy");
    if (ImGui::Button("Add GameObject"))
    {
        ImGui::OpenPopup("Add GameObject");
            
    }
    ImGui::SameLine();
    if (ImGui::Button("Add Prototype"))
    {
        ImGui::OpenPopup("Add Prototype");
    }
    Add_GameObject_Popup();
    m_pGameInstance->Object_Manager_RenderImGui();
    ImGui::End();
    return S_OK;
}

HRESULT CHierarchy::Add_GameObject_Popup()
{
    static _wstring strCurSelPrototype = {};
    if (ImGui::BeginPopupModal("Add GameObject", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        for (_uint i = 0; i < ENUM_CLASS(LEVEL::END); ++i)
        {
            ImGui::SameLine();  
            if (ImGui::Button(ToString(i)))
            {
                m_eCurSelLevel = ENUM_LEVEL(i);
            }
        }
        ImGui::Separator();

#pragma region GameObject Prototype Lists
        if (ImGui::BeginListBox(ToString(m_eCurSelLevel)))
        {
            m_pGameInstance->Render_PrototypeManagerImGui(ENUM_CLASS(m_eCurSelLevel), L"GameObject", strCurSelPrototype);
            ImGui::EndListBox();
        }
#pragma endregion

        ImGui::Separator();

		static _char szGameObjectName[MAX_PATH] = {};
        static _bool b2D = { false };
        static CUIObject::UIOBJECT_DESC tempDesc = {};
        ImGui::Checkbox("2D | 3D", &b2D);
        ImGui::InputText("GameObject Name", szGameObjectName, MAX_PATH, ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::Checkbox("Relative Transform", &tempDesc.bRelative);
        if (b2D)
        {
            ImGui::DragFloat2("Postion(X, Y)", reinterpret_cast<_float*>(&tempDesc.vPos), 1.0f);
            ImGui::DragFloat2("Size(X, Y)", reinterpret_cast<_float*>(&tempDesc.vSize), 1.0f);
        }
        else
        {
            ImGui::DragFloat3("Postion(X, Y, Z)", reinterpret_cast<_float*>(tempDesc.WorldMatrix.m[3]), 1.0f);
            tempDesc.WorldMatrix._44 = 1.0f;
            
        }
        lstrcpy(tempDesc.szName, Utf8ToWide(szGameObjectName).c_str());
		

        static char szLayerBuffer[MAX_PATH] = {};
        static _uint iGameObjectID = {};
        static char szGameObjectBuffer[MAX_PATH] = {};

        m_pGameInstance->Render_ImGuiLayerList(m_pGameInstance->Get_CurrentLevelIndex(), szLayerBuffer, iGameObjectID, MAX_PATH);

        ImGui::Spacing(); // 약간의 공백 추가

#pragma region Buttons
        if (ImGui::Button("Add", ImVec2(120, 0)))
        {
            if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(m_eCurSelLevel), strCurSelPrototype,
                m_pGameInstance->Get_CurrentLevelIndex(), Utf8ToWide(szLayerBuffer), iGameObjectID, &tempDesc)))
                return E_FAIL;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus(); // '닫기' 버튼에 기본 포커스
#pragma endregion
        ImGui::EndPopup();
    }
    return S_OK;
}

HRESULT CHierarchy::Add_Prototype_Popup()
{
    return S_OK;
}

HRESULT CHierarchy::Delete_GameObject_Popup()
{
    return S_OK;
}
