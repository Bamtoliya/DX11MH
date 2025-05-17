#include "Hierarchy.h"
#include "GameInstance.h"

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
    Add_GameObject_Popup();
    m_pGameInstance->Object_Manager_RenderImGui();
    ImGui::End();
    return S_OK;
}

HRESULT CHierarchy::Add_GameObject_Popup()
{
    //if(ImGui::Begin("Add GameObject"))
    //{
    //    //눌렀을때 프로토타입에있는 목록을 가져와서 표시하기
    //    //팝업위에는 카테고리주기 어떻게? 현재 레벨에 선언된 애들
    //    //현재 존재하는 레이어 드랍박스
    //    //없으면 텍스트 입력 받아서 새로 생성
    //    // 
    //    ImGui::End();
    //}
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

        if (ImGui::BeginListBox(ToString(m_eCurSelLevel)))
        {
            m_pGameInstance->Render_PrototypeManagerImGui(ENUM_CLASS(m_eCurSelLevel), L"GameObject", strCurSelPrototype);
            ImGui::EndListBox();
        }
        ImGui::Separator();

        static char szBuffer[MAX_PATH] = {};

        m_pGameInstance->Render_ImGuiLayerList(m_pGameInstance->Get_CurrentLevelIndex(), szBuffer, MAX_PATH);

        ImGui::Spacing(); // 약간의 공백 추가

        if (ImGui::Button("Add", ImVec2(120, 0))) 
        {
            if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(m_eCurSelLevel), strCurSelPrototype,
                m_pGameInstance->Get_CurrentLevelIndex(), Utf8ToWide(szBuffer))))
                return E_FAIL;
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

HRESULT CHierarchy::Delete_GameObject_Popup()
{
    return S_OK;
}
