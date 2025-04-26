#include "MyImGui_Manager.h"
#include "GameInstance.h"
#include "Transform_Imgui.h"

using ImGuiCallBack = function<void()>;

#pragma region HelperFuntions
#pragma endregion

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

void CMyImGui_Manager::Update(_float fTimeDelta)
{
    if (GetAsyncKeyState(VK_SHIFT) & GetAsyncKeyState(VK_TAB) & 0x8000)
    {

    }
}

HRESULT CMyImGui_Manager::Render()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    for (auto& i : m_registeredCallbacks)
    {
        if (i.callback)
            i.callback();
    }

    bool show_demo_window = true;
    bool show_another_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }

    ImGui::Render();
    /*const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
    m_pContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
    m_pContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);*/
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
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

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

#pragma region CallBack Settings
ImGuiDrawCallbackHandle CMyImGui_Manager::RegisterDrawCallback(ImGuiDrawCallBack callback, const std::string& debugName)
{
    if (!callback) { // 유효하지 않은 콜백은 등록하지 않음
        return InvalidCallbackHandle;
    }

    ImGuiDrawCallbackHandle currentHandle = m_nextHandle++;
    m_registeredCallbacks.push_back({ currentHandle, std::move(callback), debugName });
    // 필요하다면 우선순위 등에 따라 정렬할 수 있음
    return currentHandle;
}

bool CMyImGui_Manager::UnregisterDrawCallback(ImGuiDrawCallbackHandle handle)
{
    if (handle == InvalidCallbackHandle) {
        return false;
    }

    auto it = std::find_if(m_registeredCallbacks.begin(), m_registeredCallbacks.end(),
        [handle](const CALLBACK_DESC& info) {
            return info.handle == handle;
        });

    if (it != m_registeredCallbacks.end()) {
        m_registeredCallbacks.erase(it);
        return true;
    }

    return false; // 해당 핸들을 찾지 못함
}

void CMyImGui_Manager::ClearAllCallbacks()
{
    m_registeredCallbacks.clear();
}
#pragma endregion