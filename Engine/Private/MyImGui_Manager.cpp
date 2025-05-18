#include "MyImGui_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MyImGui.h"

using ImGuiCallBack = function<void()>;

namespace fs = std::filesystem;

#pragma region HelperFuntions
#pragma endregion

#pragma region Constructor/Destructor
CMyImGui_Manager::CMyImGui_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pGameInstance{ CGameInstance::Get_Instance() }
    , m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CMyImGui_Manager::Initialize(HWND hwnd)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Docking

    //io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF("../../Assets/Fonts/YES24.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesKorean());

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    return S_OK;
}

CMyImGui_Manager* CMyImGui_Manager::Create(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMyImGui_Manager* pInstance = new CMyImGui_Manager(pDevice, pContext);

    if (FAILED(pInstance->Initialize(hWnd)))
    {
        MSG_BOX("Failed to Created : CMyImGui_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMyImGui_Manager::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    for (auto& pair : m_MyImGuis)
    {
        Safe_Release(pair.second);
    }
    m_MyImGuis.clear();
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
#pragma endregion



void CMyImGui_Manager::Update(_float fTimeDelta)
{
    //for(auto& e : )
    for (auto& e : m_MyImGuis)
    {
        e.second->Update(fTimeDelta);
    }
    if (GetAsyncKeyState(VK_SHIFT) & GetAsyncKeyState(VK_TAB) & 0x8000)
    {

    }
    m_pGameObject = m_pGameInstance->Get_PickedObject();
}

HRESULT CMyImGui_Manager::Pre_Render()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    renderedObjectIDsThisFrame.clear();
    return S_OK;
}

HRESULT CMyImGui_Manager::Render()
{
    Pre_Render();

    DrawDockingSpace(&m_bDockingSpace);

    for (auto& e : m_MyImGuis)
    {
        e.second->Render();
    }
    Post_Render();
    return S_OK;
}

HRESULT CMyImGui_Manager::Post_Render()
{
    ImGui::Render();
    /*const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
    m_pContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
    m_pContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);*/
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    return S_OK;
}

void CMyImGui_Manager::DrawDockingSpace(bool* p_open)
{
    if (p_open)
    {
        // --- 도크스페이스 생성 (메뉴바 다음에 호출) ---
    // 메인 뷰포트 가져오기
        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        // 도킹 공간 플래그
        // 메뉴바가 있을 경우 DockSpaceOverViewport는 자동으로 메뉴바 아래 영역에 맞춰 생성됩니다.
        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

        // 뷰포트 위에 도킹 공간 생성
        ImGuiID dockspace_id = ImGui::GetID("MyFullscreenDockSpace");
        // DockSpaceOverViewport 호출 시 viewport 포인터를 두 번째 인자로 전달합니다.
        ImGui::DockSpaceOverViewport(dockspace_id, viewport, dockspace_flags);
    }
}

HRESULT CMyImGui_Manager::Resize(_uint iWinSizeX, _uint iWinSizeY)
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    viewport->PlatformRequestResize = true;
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(static_cast<float>(iWinSizeX), static_cast<float>(iWinSizeY));
    // DPI 스케일링을 사용하지 않는 경우 보통 1.0f
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    return S_OK;
}

HRESULT CMyImGui_Manager::Add_ImGui(const _wstring& key, CMyImGui* pMyImGui)
{
    if (pMyImGui == nullptr)
        return E_FAIL;

    auto [iterator, success] = m_MyImGuis.try_emplace(key, pMyImGui);

    if (!success) 
        return E_FAIL;

    return S_OK;
}

HRESULT CMyImGui_Manager::Clear()
{
    // 맵이 비어있지 않은 경우에만 처리합니다.
    if (!m_MyImGuis.empty())
    {
        auto it = m_MyImGuis.begin();
        while (it != m_MyImGuis.end())
        {
            if (it->first != L"ToolBar")
            {
                Safe_Release(it->second);
                // 다음 요소로 이동하기 전에 현재 요소를 맵에서 제거합니다.
                // map::erase는 제거된 요소 다음의 반복자를 반환합니다.
                it = m_MyImGuis.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
    return S_OK;
}

HRESULT CMyImGui_Manager::ToggleShowImGui(const _wstring& key)
{
    if (m_MyImGuis.find(key) != m_MyImGuis.end())
        m_MyImGuis[key]->ToggleShow();
    return S_OK;
}

HRESULT CMyImGui_Manager::ShowImGui(const _wstring& key)
{
    if (m_MyImGuis.find(key) != m_MyImGuis.end())
        m_MyImGuis[key]->Show();
    return S_OK;
}

HRESULT CMyImGui_Manager::HideImGui(const _wstring& key)
{
    if (m_MyImGuis.find(key) != m_MyImGuis.end())
        m_MyImGuis[key]->Hide();
    return S_OK;
}

HRESULT CMyImGui_Manager::ShowAll()
{
    for (auto& iter : m_MyImGuis)
    {
        iter.second->Show();
    }
    return S_OK;
}

HRESULT CMyImGui_Manager::HideAll()
{
    for (auto& iter : m_MyImGuis)
    {
        iter.second->Hide();    
    }
    return S_OK;
}
